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
    DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
        : DataFlowGraphModel(std::move(registry))
    {}

    NodeId addNode(QString const nodeType) override
    {
        if (nodeType == "VideoOutput") {
            auto it = nodesMap.find(NodeTypes::Video_Output);
            if (it != nodesMap.end()) {
                const std::unordered_set<NodeId> &nodeSet = it->second;
                if (nodeSet.size() > 0) {
                    return InvalidNodeId;
                }
            }
        }
        NodeId newNodeId = DataFlowGraphModel::addNode(nodeType);
        if (nodeType == "Process") {
            _nodePortCounts[newNodeId].in = 1;
            widget(newNodeId)->populateButtons(PortType::In, 1);
            _nodePortCounts[newNodeId].out = 1;
            widget(newNodeId)->populateButtons(PortType::Out, 1);
            _nodeSize[newNodeId] = QSize(250, 130);
        }
        return newNodeId;
    }

    bool detachPossible(ConnectionId const) const override { return true; }

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
        auto nodeType = stringToNodeType(nodeTypeName.toString());
        if (nodeTypeName == "VideoOutput") {
            return false;
        } else if (nodeTypeName == "VideoInput") {
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
        _nodeWidgets.erase(nodeId);
        _nodePortCounts.erase(nodeId);
        if (nodeType) {
            NodeTypes type = *nodeType;
            auto it = nodesMap.find(type);
            if (it != nodesMap.end()) {
                it->second.erase(nodeId);
            }
        }

        return DataFlowGraphModel::deleteNode(nodeId);
    }

    QVariant nodeData(NodeId nodeId, NodeRole role) const override
    {
        QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
        if (nodeTypeName == "Process") {
            switch (role) {
            case NodeRole::Size:
                return _nodeSize[nodeId];
            case NodeRole::InPortCount:
                return _nodePortCounts[nodeId].in;

            case NodeRole::OutPortCount:
                return _nodePortCounts[nodeId].out;

            case NodeRole::Widget: {
                return QVariant::fromValue(widget(nodeId));
            }
            }
        }

        return DataFlowGraphModel::nodeData(nodeId, role);
    }

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override
    {
        QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
        if (nodeTypeName == "Process") {
            switch (role) {
            case NodeRole::Size:
                _nodeSize[nodeId] = value.value<QSize>();
                return true;
            case NodeRole::InPortCount:
                _nodePortCounts[nodeId].in = value.toUInt();
                widget(nodeId)->populateButtons(PortType::In, value.toUInt());
                return false;

            case NodeRole::OutPortCount:
                _nodePortCounts[nodeId].out = value.toUInt();
                widget(nodeId)->populateButtons(PortType::Out, value.toUInt());
                return false;

            case NodeRole::Widget:
                return false;
            }
        }

        return DataFlowGraphModel::setNodeData(nodeId, role, value);
    }

    void addPort(NodeId nodeId, PortType portType, PortIndex portIndex)
    {
        // STAGE 1.
        // Compute new addresses for the existing connections that are shifted and
        // placed after the new ones
        PortIndex first = portIndex;
        PortIndex last = first;
        portsAboutToBeInserted(nodeId, portType, first, last);

        // STAGE 2. Change the number of connections in your model
        if (portType == PortType::In)
            _nodePortCounts[nodeId].in++;
        else
            _nodePortCounts[nodeId].out++;

        // STAGE 3. Re-create previouly existed and now shifted connections
        portsInserted();

        Q_EMIT nodeUpdated(nodeId);
    }

    void removePort(NodeId nodeId, PortType portType, PortIndex portIndex)
    {
        // STAGE 1.
        // Compute new addresses for the existing connections that are shifted upwards
        // instead of the deleted ports.
        PortIndex first = portIndex;
        PortIndex last = first;
        portsAboutToBeDeleted(nodeId, portType, first, last);

        // STAGE 2. Change the number of connections in your model
        if (portType == PortType::In)
            _nodePortCounts[nodeId].in--;
        else
            _nodePortCounts[nodeId].out--;

        portsDeleted();

        Q_EMIT nodeUpdated(nodeId);
    }

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
    struct NodePortCount
    {
        unsigned int in = 0;
        unsigned int out = 0;
    };
    mutable std::unordered_map<NodeId, NodePortCount> _nodePortCounts;
    mutable std::unordered_map<NodeId, PortAddRemoveWidget *> _nodeWidgets;
    mutable std::unordered_map<NodeId, QSize> _nodeSize;
    PortAddRemoveWidget *widget(NodeId nodeId) const
    {
        auto it = _nodeWidgets.find(nodeId);
        if (it == _nodeWidgets.end()) {
            _nodeWidgets[nodeId] = new PortAddRemoveWidget(nodeId,
                                                           *const_cast<DataFlowModel *>(this));
        }

        return _nodeWidgets[nodeId];
    }
};
