#pragma once

#include <QtNodes/DataFlowGraphModel>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::InvalidNodeId;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeFlag;
using QtNodes::NodeFlags;
using QtNodes::NodeId;

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
    DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
        : DataFlowGraphModel(std::move(registry))
    {}

    bool detachPossible(ConnectionId const) const override { return true; }

    /*NodeFlags nodeFlags(NodeId nodeId) const override
    {
        auto basicFlags = DataFlowGraphModel::nodeFlags(nodeId);
        QVariant nodeTypeName = nodeData(nodeId, QtNodes::NodeRole::Type);

        return basicFlags;
    }*/

    NodeId addNodeType(NodeTypes type)
    {
        QString nodeTypeName = nodeTypeToName[type];
        NodeId newNodeId = addNode(nodeTypeName);
        nodesMap[type].insert(newNodeId);
        return newNodeId;
    }

    NodeId addNodeName(QString const nodeTypeName)
    {
        auto nodeType = stringToNodeType(nodeTypeName);
        if (nodeType) {
            NodeTypes type = *nodeType;
            return addNodeType(type);
        }
        return QtNodes::InvalidNodeId;
    }

    bool deleteNode(NodeId const nodeId) override
    {
        QVariant nodeTypeName = nodeData(nodeId, QtNodes::NodeRole::Type);
        if (nodeTypeName == "VideoOutput") {
            return false;
        } else if (nodeTypeName == "VideoInput") {
            auto nodeType = stringToNodeType(nodeTypeName.toString());
            if (nodeType) {
                NodeTypes type = *nodeType;
                auto it = nodesMap.find(type);
                if (it != nodesMap.end()) {
                    const std::unordered_set<NodeId> &nodeSet = it->second;
                    if (nodeSet.size() == 1) {
                        return false;
                    }
                }
            }
        }
        return DataFlowGraphModel::deleteNode(nodeId);
    }

    bool deleteConnection(ConnectionId const connectionId) override { return false; }

private:
    std::unordered_map<NodeTypes, std::unordered_set<NodeId>> nodesMap;

    std::optional<NodeTypes> stringToNodeType(const QString &str) const
    {
        static const QHash<QString, NodeTypes> map = [] {
            QHash<QString, NodeTypes> m;
            for (auto it = nodeTypeToName.begin(); it != nodeTypeToName.end(); ++it)
                m[it.value()] = it.key();
            return m;
        }();

        auto it = map.find(str);
        if (it != map.end()) {
            return it.value();
        } else {
            return std::nullopt;
        }
    }
};
