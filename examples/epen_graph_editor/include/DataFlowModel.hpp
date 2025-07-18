#pragma once

#include "PortAddRemoveWidget.hpp"
#include "data_models/OperationDataModel.hpp"
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

enum class NodeTypes {
    Video_Input,
    Video_Output,
    FixBuffer,
    Process,
    InBuffer,
    OutBuffer,
    InImage,
    OutImage,
    SliderBuffer,
    CheckboxBuffer,
    Color4Buffer,
    PlainNumberBuffer
};

const QMap<NodeTypes, QString> nodeTypeToName = {
    {NodeTypes::Video_Input, "VideoInput"},
    {NodeTypes::Video_Output, "VideoOutput"},
    {NodeTypes::FixBuffer, "FixBuffer"},
    {NodeTypes::Process, "Process"},
    {NodeTypes::InBuffer, "InBuffer"},
    {NodeTypes::OutBuffer, "OutBuffer"},
    {NodeTypes::SliderBuffer, "SliderBuffer"},
    {NodeTypes::InImage, "InImage"},
    {NodeTypes::OutImage, "OutImage"},
    {NodeTypes::CheckboxBuffer, "CheckboxBuffer"},
    {NodeTypes::Color4Buffer, "Color4Buffer"},
    {NodeTypes::PlainNumberBuffer, "PlainNumberBuffer"},
};

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

    void addProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex);

    void removeProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex);

private:
    QString generateNewNodeName(NodeTypes nodeType, QString typeNamePrefix);
    std::optional<NodeTypes> stringToNodeType(const QString &str) const;

    std::unordered_map<NodeTypes, std::unordered_set<NodeId>> nodesMap;
    struct NodePortCount
    {
        unsigned int in = 0;
        unsigned int out = 0;
    };
    mutable std::unordered_map<NodeId, NodePortCount> _nodePortCounts;
    mutable std::unordered_map<NodeId, PortAddRemoveWidget *> _nodeWidgets;
    mutable std::unordered_map<NodeId, QSize> _nodeSize;
    mutable std::unordered_map<NodeId, QString> _nodeNames;
    PortAddRemoveWidget *widget(NodeId nodeId) const;
};
