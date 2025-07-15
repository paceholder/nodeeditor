#pragma once

#include "PortAddRemoveWidget.hpp"
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

enum class NodeTypes { Video_Input, Video_Output, Image, Buffer, Process };

const QMap<NodeTypes, QString> nodeTypeToName = {{NodeTypes::Video_Input, "VideoInput"},
                                                 {NodeTypes::Video_Output, "VideoOutput"},
                                                 {NodeTypes::Image, "Image"},
                                                 {NodeTypes::Buffer, "Buffer"},
                                                 {NodeTypes::Process, "Process"}};

namespace std {
template<>
struct hash<NodeTypes>
{
    std::size_t operator()(NodeTypes c) const { return static_cast<std::size_t>(c); }
};
} // namespace std

class DataFlowModel : public DataFlowGraphModel
{
public:
    DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry);

    NodeId addNode(QString const nodeType) override;
   

    bool detachPossible(ConnectionId const) const override { return true; }

    NodeId addNodeType(NodeTypes type);

    NodeId addNodeName(QString const nodeTypeName);

    bool deleteNode(NodeId const nodeId) override;
    

    QVariant nodeData(NodeId nodeId, NodeRole role) const override;
    

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;
    

    void addPort(NodeId nodeId, PortType portType, PortIndex portIndex);

    void removePort(NodeId nodeId, PortType portType, PortIndex portIndex);

private:
    std::unordered_map<NodeTypes, std::unordered_set<NodeId>> nodesMap;

    std::optional<NodeTypes> stringToNodeType(const QString &str) const;
    struct NodePortCount
    {
        unsigned int in = 0;
        unsigned int out = 0;
    };
    mutable std::unordered_map<NodeId, NodePortCount> _nodePortCounts;
    mutable std::unordered_map<NodeId, PortAddRemoveWidget *> _nodeWidgets;
    mutable std::unordered_map<NodeId, QSize> _nodeSize;
    PortAddRemoveWidget *widget(NodeId nodeId) const;
};
