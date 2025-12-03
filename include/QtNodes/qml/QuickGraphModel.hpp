#pragma once

#include <QtCore/QObject>
#include <memory>

#include "NodesListModel.hpp"
#include "ConnectionsListModel.hpp"

namespace QtNodes {

class DataFlowGraphModel;
class NodeDelegateModelRegistry;

class QuickGraphModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtNodes::NodesListModel* nodes READ nodes CONSTANT)
    Q_PROPERTY(QtNodes::ConnectionsListModel* connections READ connections CONSTANT)

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

private:
    std::shared_ptr<DataFlowGraphModel> _model;
    NodesListModel* _nodesList;
    ConnectionsListModel* _connectionsList;
};

} // namespace QtNodes
