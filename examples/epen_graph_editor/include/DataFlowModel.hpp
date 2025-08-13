#pragma once

#include "PortAddRemoveWidget.hpp"
#include "data_models/OperationDataModel.hpp"
#include "data_models/Process.hpp"
#include "panels/FloatingProperties.hpp"
#include "ProcessPort.hpp"
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
    void addConnection(ConnectionId const connectionId) override;
    bool deleteConnection(ConnectionId const connectionId) override;

    bool detachPossible(ConnectionId const) const override { return true; }

    bool deleteNode(NodeId const nodeId) override;

    QVariant nodeData(NodeId nodeId, NodeRole role) const override;

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    void addProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex, ProcessPort *port);

    void removeProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex);
    void setFloatingProperties(QPointer<FloatingProperties>);

    float getlastProcessLeft();

    QPair<float, float> getProcessNodeRange(NodeId nodeId, QPointF currentPos);

    void setSelectedNode(OperationDataModel *, NodeId);
    void deselectNode();

    void setSelectedPort(NodeId nodeId, bool isRightPort, int portIndex);

    void notifyPortInsertion(NodeId nodeId);

    PortAddRemoveWidget *widget(NodeId nodeId) const;

    void addProcessPort(NodeId nodeId, bool isRight, bool isImage);

    QString getInputImagePortName();
    QString getInputBufferPortName();
    QString getOutputImagePortName();
    QString getOutputBufferPortName();
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

    QPointer<FloatingProperties> _propertyPanel;

    int _inputImagePortCount = 0;
    int _inputBufferPortCount = 0;
    int _outputImagePortCount = 0;
    int _outputBufferPortCount = 0;
};
