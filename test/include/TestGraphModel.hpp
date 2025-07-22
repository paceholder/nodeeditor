#pragma once

#include <QtNodes/AbstractGraphModel>

#include <QPointF>
#include <QJsonObject>
#include <QSizeF>
#include <QSizeF>

#include <unordered_map>
#include <unordered_set>

using QtNodes::AbstractGraphModel;
using QtNodes::ConnectionId;
using QtNodes::NodeFlags;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortIndex;
using QtNodes::PortRole;
using QtNodes::PortType;

/**
 * @brief A simple test implementation of AbstractGraphModel for unit testing.
 */
class TestGraphModel : public AbstractGraphModel
{
    Q_OBJECT

public:
    TestGraphModel() : AbstractGraphModel() {}

    NodeId newNodeId() override { return _nextNodeId++; }

    std::unordered_set<NodeId> allNodeIds() const override { return _nodeIds; }

    std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override
    {
        std::unordered_set<ConnectionId> result;
        for (const auto &conn : _connections) {
            if (conn.inNodeId == nodeId || conn.outNodeId == nodeId) {
                result.insert(conn);
            }
        }
        return result;
    }

    std::unordered_set<ConnectionId> connections(NodeId nodeId,
                                                 PortType portType,
                                                 PortIndex portIndex) const override
    {
        std::unordered_set<ConnectionId> result;
        for (const auto &conn : _connections) {
            if (portType == PortType::In && conn.inNodeId == nodeId && conn.inPortIndex == portIndex) {
                result.insert(conn);
            } else if (portType == PortType::Out && conn.outNodeId == nodeId
                       && conn.outPortIndex == portIndex) {
                result.insert(conn);
            }
        }
        return result;
    }

    bool connectionExists(ConnectionId const connectionId) const override
    {
        return _connections.find(connectionId) != _connections.end();
    }

    NodeId addNode(QString const nodeType = QString()) override
    {
        NodeId id = newNodeId();
        _nodeIds.insert(id);
        _nodeData[id][NodeRole::Type] = nodeType;
        _nodeData[id][NodeRole::Position] = QPointF(0, 0);
        _nodeData[id][NodeRole::Caption] = QString("Node %1").arg(id);
        _nodeData[id][NodeRole::InPortCount] = 1u;
        _nodeData[id][NodeRole::OutPortCount] = 1u;
        Q_EMIT nodeCreated(id);
        return id;
    }

    bool connectionPossible(ConnectionId const connectionId) const override
    {
        // Basic validation: nodes exist and not connecting to self
        return nodeExists(connectionId.inNodeId) && nodeExists(connectionId.outNodeId)
               && connectionId.inNodeId != connectionId.outNodeId;
    }

    void addConnection(ConnectionId const connectionId) override
    {
        if (connectionPossible(connectionId)) {
            _connections.insert(connectionId);
            Q_EMIT connectionCreated(connectionId);
        }
    }

    bool nodeExists(NodeId const nodeId) const override
    {
        return _nodeIds.find(nodeId) != _nodeIds.end();
    }

    QVariant nodeData(NodeId nodeId, NodeRole role) const override
    {
        auto nodeIt = _nodeData.find(nodeId);
        if (nodeIt != _nodeData.end()) {
            auto roleIt = nodeIt->second.find(role);
            if (roleIt != nodeIt->second.end()) {
                return roleIt->second;
            }
        }
        
        // Provide default values for essential display properties
        switch (role) {
        case NodeRole::Type:
            return QString("TestNode");
            
        case NodeRole::Caption:
            return QString("Test Node %1").arg(nodeId);
            
        case NodeRole::CaptionVisible:
            return true;
            
        case NodeRole::Size:
            return QSizeF(120, 80);
            
        case NodeRole::Position:
            return QPointF(0, 0); // Default position if none set
            
        default:
            break;
        }
        
        return QVariant();
    }

    // Make the template version from the base class available
    using AbstractGraphModel::nodeData;

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override
    {
        if (nodeExists(nodeId)) {
            _nodeData[nodeId][role] = value;
            
            // Only emit specific signals for user-initiated changes
            // Don't emit for computed/internal roles to avoid recursion
            switch (role) {
            case NodeRole::Position:
                Q_EMIT nodePositionUpdated(nodeId);
                break;
            case NodeRole::Type:
            case NodeRole::Caption:
            case NodeRole::CaptionVisible:
            case NodeRole::InPortCount:
            case NodeRole::OutPortCount:
                Q_EMIT nodeUpdated(nodeId);
                break;
            case NodeRole::Size:
            case NodeRole::Style:
            case NodeRole::InternalData:
            case NodeRole::Widget:
                // These are often computed/internal - don't emit signals
                break;
            }
            return true;
        }
        return false;
    }

    QVariant portData(NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      PortRole role) const override
    {
        Q_UNUSED(nodeId)
        Q_UNUSED(portType)
        Q_UNUSED(portIndex)
        Q_UNUSED(role)
        return QVariant();
    }

    bool setPortData(NodeId nodeId,
                     PortType portType,
                     PortIndex portIndex,
                     QVariant const &value,
                     PortRole role = PortRole::Data) override
    {
        Q_UNUSED(nodeId)
        Q_UNUSED(portType)
        Q_UNUSED(portIndex)
        Q_UNUSED(value)
        Q_UNUSED(role)
        return false;
    }

    bool deleteConnection(ConnectionId const connectionId) override
    {
        auto it = _connections.find(connectionId);
        if (it != _connections.end()) {
            _connections.erase(it);
            Q_EMIT connectionDeleted(connectionId);
            return true;
        }
        return false;
    }

    bool deleteNode(NodeId const nodeId) override
    {
        if (!nodeExists(nodeId))
            return false;

        // Remove all connections involving this node
        std::vector<ConnectionId> connectionsToRemove;
        for (const auto &conn : _connections) {
            if (conn.inNodeId == nodeId || conn.outNodeId == nodeId) {
                connectionsToRemove.push_back(conn);
            }
        }

        for (const auto &conn : connectionsToRemove) {
            deleteConnection(conn);
        }

        // Remove the node
        _nodeIds.erase(nodeId);
        _nodeData.erase(nodeId);
        Q_EMIT nodeDeleted(nodeId);
        return true;
    }

    QJsonObject saveNode(NodeId const nodeId) const override
    {
        QJsonObject result;
        result["id"] = static_cast<int>(nodeId);
        auto nodeIt = _nodeData.find(nodeId);
        if (nodeIt != _nodeData.end()) {
            const auto &data = nodeIt->second;
            auto posIt = data.find(NodeRole::Position);
            if (posIt != data.end()) {
                QPointF pos = posIt->second.toPointF();
                QJsonObject posObj;
                posObj["x"] = pos.x();
                posObj["y"] = pos.y();
                result["position"] = posObj;
            }
        }
        return result;
    }

private:
    NodeId _nextNodeId = 1;
    std::unordered_set<NodeId> _nodeIds;
    std::unordered_set<ConnectionId> _connections;
    std::unordered_map<NodeId, std::unordered_map<NodeRole, QVariant>> _nodeData;
};
