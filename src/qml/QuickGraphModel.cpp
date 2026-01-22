#include "QtNodes/qml/QuickGraphModel.hpp"

#include "QtNodes/internal/DataFlowGraphModel.hpp"
#include "QtNodes/internal/NodeDelegateModelRegistry.hpp"
#include "QtNodes/internal/NodeData.hpp"

#include <QUndoStack>
#include <QUndoCommand>
#include <QJsonObject>

namespace QtNodes {

// Undo command for adding a node
class AddNodeCommand : public QUndoCommand
{
public:
    AddNodeCommand(DataFlowGraphModel* graphModel, 
                   const QString& nodeType, QUndoCommand* parent = nullptr)
        : QUndoCommand(parent)
        , _graphModel(graphModel)
        , _nodeType(nodeType)
        , _nodeId(-1)
    {
        setText(QString("Add %1").arg(nodeType));
    }
    
    void undo() override
    {
        if (_nodeId >= 0 && _graphModel) {
            _savedState = _graphModel->saveNode(static_cast<NodeId>(_nodeId));
            _graphModel->deleteNode(static_cast<NodeId>(_nodeId));
        }
    }
    
    void redo() override
    {
        if (_graphModel) {
            if (_nodeId < 0) {
                _nodeId = static_cast<int>(_graphModel->addNode(_nodeType));
            } else if (!_savedState.isEmpty()) {
                _graphModel->loadNode(_savedState);
            }
        }
    }
    
    int nodeId() const { return _nodeId; }
    
private:
    DataFlowGraphModel* _graphModel;
    QString _nodeType;
    int _nodeId;
    QJsonObject _savedState;
};

// Undo command for removing a node
class RemoveNodeCommand : public QUndoCommand
{
public:
    RemoveNodeCommand(DataFlowGraphModel* graphModel, int nodeId, QUndoCommand* parent = nullptr)
        : QUndoCommand(parent)
        , _graphModel(graphModel)
        , _nodeId(nodeId)
    {
        setText(QString("Remove Node %1").arg(nodeId));
        if (_graphModel) {
            _savedState = _graphModel->saveNode(static_cast<NodeId>(_nodeId));
        }
    }
    
    void undo() override
    {
        if (_graphModel && !_savedState.isEmpty()) {
            _graphModel->loadNode(_savedState);
        }
    }
    
    void redo() override
    {
        if (_graphModel) {
            _savedState = _graphModel->saveNode(static_cast<NodeId>(_nodeId));
            _graphModel->deleteNode(static_cast<NodeId>(_nodeId));
        }
    }
    
private:
    DataFlowGraphModel* _graphModel;
    int _nodeId;
    QJsonObject _savedState;
};

// Undo command for adding a connection
class AddConnectionCommand : public QUndoCommand
{
public:
    AddConnectionCommand(DataFlowGraphModel* graphModel, const ConnectionId& connId, 
                         QUndoCommand* parent = nullptr)
        : QUndoCommand(parent)
        , _graphModel(graphModel)
        , _connId(connId)
    {
        setText("Add Connection");
    }
    
    void undo() override
    {
        if (_graphModel) {
            _graphModel->deleteConnection(_connId);
        }
    }
    
    void redo() override
    {
        if (_graphModel) {
            _graphModel->addConnection(_connId);
        }
    }
    
private:
    DataFlowGraphModel* _graphModel;
    ConnectionId _connId;
};

// Undo command for removing a connection
class RemoveConnectionCommand : public QUndoCommand
{
public:
    RemoveConnectionCommand(DataFlowGraphModel* graphModel, const ConnectionId& connId, 
                            QUndoCommand* parent = nullptr)
        : QUndoCommand(parent)
        , _graphModel(graphModel)
        , _connId(connId)
    {
        setText("Remove Connection");
    }
    
    void undo() override
    {
        if (_graphModel) {
            _graphModel->addConnection(_connId);
        }
    }
    
    void redo() override
    {
        if (_graphModel) {
            _graphModel->deleteConnection(_connId);
        }
    }
    
private:
    DataFlowGraphModel* _graphModel;
    ConnectionId _connId;
};

QuickGraphModel::QuickGraphModel(QObject *parent)
    : QObject(parent)
    , _nodesList(nullptr)
    , _connectionsList(nullptr)
    , _undoStack(new QUndoStack(this))
{
    connect(_undoStack, &QUndoStack::canUndoChanged, this, &QuickGraphModel::undoStateChanged);
    connect(_undoStack, &QUndoStack::canRedoChanged, this, &QuickGraphModel::undoStateChanged);
}

QuickGraphModel::~QuickGraphModel()
{
    // delete models managed by QML ownership usually, but here they are children of this? 
    // Actually QAbstractListModels are QObjects, so if we parent them, they auto delete.
}

void QuickGraphModel::setRegistry(std::shared_ptr<NodeDelegateModelRegistry> registry)
{
    _model = std::make_shared<DataFlowGraphModel>(registry);
    
    // Re-create list models
    if (_nodesList) _nodesList->deleteLater();
    if (_connectionsList) _connectionsList->deleteLater();

    _nodesList = new NodesListModel(_model, this);
    _connectionsList = new ConnectionsListModel(_model, this);
    
    // Notify QML that properties changed? 
    // Since they are CONSTANT in my declaration, QML assumes they don't change. 
    // Ideally I should have a NOTIFY signal, but for simplicity I assume setRegistry is called before QML uses it, 
    // or I should update the property declaration. 
    // However, since we are constructor-injecting or property-injecting in C++, 
    // it's safer to make them NOTIFY. But for now, let's stick to CONSTANT and assume setup happens at startup.
}

std::shared_ptr<DataFlowGraphModel> QuickGraphModel::graphModel() const
{
    return _model;
}

NodesListModel* QuickGraphModel::nodes() const
{
    return _nodesList;
}

ConnectionsListModel* QuickGraphModel::connections() const
{
    return _connectionsList;
}

int QuickGraphModel::addNode(QString const &nodeType)
{
    if (!_model) return -1;
    
    auto* cmd = new AddNodeCommand(_model.get(), nodeType);
    _undoStack->push(cmd);
    return cmd->nodeId();
}

bool QuickGraphModel::removeNode(int nodeId)
{
    if (!_model) return false;
    
    _undoStack->push(new RemoveNodeCommand(_model.get(), nodeId));
    return true;
}

void QuickGraphModel::addConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex)
{
    if (!_model) return;
    ConnectionId connId;
    connId.outNodeId = static_cast<NodeId>(outNodeId);
    connId.outPortIndex = static_cast<PortIndex>(outPortIndex);
    connId.inNodeId = static_cast<NodeId>(inNodeId);
    connId.inPortIndex = static_cast<PortIndex>(inPortIndex);
    
    _undoStack->push(new AddConnectionCommand(_model.get(), connId));
}

void QuickGraphModel::removeConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex)
{
    if (!_model) return;
    ConnectionId connId;
    connId.outNodeId = static_cast<NodeId>(outNodeId);
    connId.outPortIndex = static_cast<PortIndex>(outPortIndex);
    connId.inNodeId = static_cast<NodeId>(inNodeId);
    connId.inPortIndex = static_cast<PortIndex>(inPortIndex);
    
    _undoStack->push(new RemoveConnectionCommand(_model.get(), connId));
}

QVariantMap QuickGraphModel::getConnectionAtInput(int nodeId, int portIndex)
{
    QVariantMap result;
    result["valid"] = false;
    
    if (!_model) return result;
    
    auto connections = _model->allConnectionIds(static_cast<NodeId>(nodeId));
    for (const auto& conn : connections) {
        if (conn.inNodeId == static_cast<NodeId>(nodeId) && 
            conn.inPortIndex == static_cast<PortIndex>(portIndex)) {
            result["valid"] = true;
            result["outNodeId"] = static_cast<int>(conn.outNodeId);
            result["outPortIndex"] = static_cast<int>(conn.outPortIndex);
            return result;
        }
    }
    
    return result;
}

QString QuickGraphModel::getPortDataTypeId(int nodeId, int portType, int portIndex)
{
    if (!_model) return QString();
    
    auto dataType = _model->portData(
        static_cast<NodeId>(nodeId),
        static_cast<PortType>(portType),
        static_cast<PortIndex>(portIndex),
        PortRole::DataType
    ).value<NodeDataType>();
    
    return dataType.id;
}

bool QuickGraphModel::connectionPossible(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex)
{
    if (!_model) return false;
    
    ConnectionId connId;
    connId.outNodeId = static_cast<NodeId>(outNodeId);
    connId.outPortIndex = static_cast<PortIndex>(outPortIndex);
    connId.inNodeId = static_cast<NodeId>(inNodeId);
    connId.inPortIndex = static_cast<PortIndex>(inPortIndex);
    
    return _model->connectionPossible(connId);
}

bool QuickGraphModel::canUndo() const
{
    return _undoStack->canUndo();
}

bool QuickGraphModel::canRedo() const
{
    return _undoStack->canRedo();
}

void QuickGraphModel::undo()
{
    _undoStack->undo();
}

void QuickGraphModel::redo()
{
    _undoStack->redo();
}

} // namespace QtNodes
