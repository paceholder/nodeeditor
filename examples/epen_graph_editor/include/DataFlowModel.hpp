#pragma once

#include "panels/FloatingProperties.hpp"
#include "PortAddRemoveWidget.hpp"
#include "data_models/OperationDataModel.hpp"
#include "data_models/Process.hpp"
#include <QPointer>
#include <QtNodes/DataFlowGraphModel>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::InvalidNodeId;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeFlag;
using QtNodes::NodeFlags;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DataFlowModel : public DataFlowGraphModel
{
    Q_OBJECT

public:
    DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry>);

    NodeId addNode(QString const nodeType) override;

    bool detachPossible(ConnectionId const) const override { return true; }

    bool deleteNode(NodeId const nodeId) override;

    QVariant nodeData(NodeId nodeId, NodeRole role) const override;

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    void addProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex, bool isImage);

    void removeProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex);
    void setFloatingProperties(QPointer<FloatingProperties>);

    float getlastProcessLeft();

    QPair<float, float> getProcessNodeRange(NodeId nodeId, QPointF currentPos);

    void setSelectedNode(OperationDataModel *, NodeId);
    void deselectNode();

    void setSelectedPort(NodeId nodeId, bool isRightPort, int portIndex);
signals:
    void nodePortSelected(bool isRightPort, Process *node, int portIndex);

private:
    QString generateNewNodeName(QString typeNamePrefix);
    std::unordered_map<QString, std::unordered_set<NodeId>> nodesMap;
    struct NodePortCount
    {
        unsigned int in = 0;
        unsigned int out = 0;
    };
    mutable std::unordered_map<NodeId, NodePortCount> _nodePortCounts;
    mutable std::unordered_map<NodeId, PortAddRemoveWidget *> _nodeWidgets;
    mutable std::unordered_map<NodeId, QSize> _processNodeSize;
    mutable std::unordered_map<NodeId, QString> _nodeNames;
    PortAddRemoveWidget *widget(NodeId nodeId) const;

    QPointer<FloatingProperties> _propertyPanel;
};
