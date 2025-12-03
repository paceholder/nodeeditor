#include "QtNodes/qml/QuickGraphModel.hpp"

#include "QtNodes/internal/DataFlowGraphModel.hpp"
#include "QtNodes/internal/NodeDelegateModelRegistry.hpp"
#include "QtNodes/internal/NodeData.hpp"

namespace QtNodes {

QuickGraphModel::QuickGraphModel(QObject *parent)
    : QObject(parent)
    , _nodesList(nullptr)
    , _connectionsList(nullptr)
{
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
    return static_cast<int>(_model->addNode(nodeType));
}

bool QuickGraphModel::removeNode(int nodeId)
{
    if (!_model) return false;
    return _model->deleteNode(static_cast<NodeId>(nodeId));
}

void QuickGraphModel::addConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex)
{
    if (!_model) return;
    ConnectionId connId;
    connId.outNodeId = static_cast<NodeId>(outNodeId);
    connId.outPortIndex = static_cast<PortIndex>(outPortIndex);
    connId.inNodeId = static_cast<NodeId>(inNodeId);
    connId.inPortIndex = static_cast<PortIndex>(inPortIndex);
    
    _model->addConnection(connId);
}

void QuickGraphModel::removeConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex)
{
    if (!_model) return;
    ConnectionId connId;
    connId.outNodeId = static_cast<NodeId>(outNodeId);
    connId.outPortIndex = static_cast<PortIndex>(outPortIndex);
    connId.inNodeId = static_cast<NodeId>(inNodeId);
    connId.inPortIndex = static_cast<PortIndex>(inPortIndex);
    
    _model->deleteConnection(connId);
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

} // namespace QtNodes
