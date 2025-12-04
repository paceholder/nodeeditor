#pragma once

#include <QtCore/QObject>
#include <memory>

#include "NodesListModel.hpp"
#include "ConnectionsListModel.hpp"
#include "QtNodes/internal/Export.hpp"

class QUndoStack;

namespace QtNodes {

class DataFlowGraphModel;
class NodeDelegateModelRegistry;

class NODE_EDITOR_PUBLIC QuickGraphModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtNodes::NodesListModel* nodes READ nodes CONSTANT)
    Q_PROPERTY(QtNodes::ConnectionsListModel* connections READ connections CONSTANT)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY undoStateChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY undoStateChanged)

public:
    explicit QuickGraphModel(QObject *parent = nullptr);
    ~QuickGraphModel();

    // Initialization with an existing registry
    void setRegistry(std::shared_ptr<NodeDelegateModelRegistry> registry);
    
    // Or just access the internal model
    std::shared_ptr<DataFlowGraphModel> graphModel() const;

    NodesListModel* nodes() const;
    ConnectionsListModel* connections() const;

    Q_INVOKABLE int addNode(QString const &nodeType);
    Q_INVOKABLE bool removeNode(int nodeId);
    
    Q_INVOKABLE void addConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex);
    Q_INVOKABLE void removeConnection(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex);
    Q_INVOKABLE QVariantMap getConnectionAtInput(int nodeId, int portIndex);
    Q_INVOKABLE QString getPortDataTypeId(int nodeId, int portType, int portIndex);
    Q_INVOKABLE bool connectionPossible(int outNodeId, int outPortIndex, int inNodeId, int inPortIndex);
    
    // Undo/Redo
    bool canUndo() const;
    bool canRedo() const;
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

Q_SIGNALS:
    void undoStateChanged();

private:
    std::shared_ptr<DataFlowGraphModel> _model;
    NodesListModel* _nodesList;
    ConnectionsListModel* _connectionsList;
    QUndoStack* _undoStack;
};

} // namespace QtNodes
