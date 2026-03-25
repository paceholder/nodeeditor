#pragma once

#include "ConnectionIdHash.hpp"

#include <unordered_map>
#include <unordered_set>

namespace QtNodes {

class ConnectionIdIndex
{
public:
    using ConnectionSet = std::unordered_set<ConnectionId>;
    using ConnectionsByPort = std::unordered_map<PortIndex, ConnectionSet>;

public:
    ConnectionSet const &connectivity() const noexcept { return _connectivity; }

    ConnectionSet const &allConnectionIds(NodeId const nodeId) const noexcept
    {
        auto it = _nodeConnections.find(nodeId);
        if (it == _nodeConnections.end()) {
            return emptyConnections();
        }

        return it->second;
    }

    ConnectionSet const &connections(NodeId nodeId, PortType portType, PortIndex portIndex) const noexcept
    {
        if (portType == PortType::None) {
            return emptyConnections();
        }

        auto const &connectionsByPort = (portType == PortType::In) ? _inConnectionsByPort
                                                                   : _outConnectionsByPort;
        auto nodeIt = connectionsByPort.find(nodeId);
        if (nodeIt == connectionsByPort.end()) {
            return emptyConnections();
        }

        auto portIt = nodeIt->second.find(portIndex);
        if (portIt == nodeIt->second.end()) {
            return emptyConnections();
        }

        return portIt->second;
    }

    bool contains(ConnectionId const connectionId) const noexcept
    {
        return _connectivity.find(connectionId) != _connectivity.end();
    }

    void add(ConnectionId const connectionId)
    {
        if (_connectivity.insert(connectionId).second) {
            indexConnection(connectionId);
        }
    }

    bool remove(ConnectionId const connectionId)
    {
        auto it = _connectivity.find(connectionId);
        if (it == _connectivity.end()) {
            return false;
        }

        _connectivity.erase(it);
        unindexConnection(connectionId);
        return true;
    }

private:
    static ConnectionSet const &emptyConnections() noexcept
    {
        static ConnectionSet const empty{};
        return empty;
    }

    void indexConnection(ConnectionId const connectionId)
    {
        _nodeConnections[connectionId.inNodeId].insert(connectionId);
        _nodeConnections[connectionId.outNodeId].insert(connectionId);
        _inConnectionsByPort[connectionId.inNodeId][connectionId.inPortIndex].insert(connectionId);
        _outConnectionsByPort[connectionId.outNodeId][connectionId.outPortIndex].insert(connectionId);
    }

    void unindexConnection(ConnectionId const connectionId)
    {
        auto eraseFromNode = [&](NodeId nodeId) {
            auto nodeIt = _nodeConnections.find(nodeId);
            if (nodeIt == _nodeConnections.end()) {
                return;
            }

            nodeIt->second.erase(connectionId);
            if (nodeIt->second.empty()) {
                _nodeConnections.erase(nodeIt);
            }
        };

        auto eraseFromPortMap =
            [&](std::unordered_map<NodeId, ConnectionsByPort> &connectionsByPort,
                NodeId nodeId,
                PortIndex portIndex) {
                auto nodeIt = connectionsByPort.find(nodeId);
                if (nodeIt == connectionsByPort.end()) {
                    return;
                }

                auto portIt = nodeIt->second.find(portIndex);
                if (portIt == nodeIt->second.end()) {
                    return;
                }

                portIt->second.erase(connectionId);
                if (portIt->second.empty()) {
                    nodeIt->second.erase(portIt);
                }

                if (nodeIt->second.empty()) {
                    connectionsByPort.erase(nodeIt);
                }
            };

        eraseFromNode(connectionId.inNodeId);
        eraseFromNode(connectionId.outNodeId);
        eraseFromPortMap(_inConnectionsByPort, connectionId.inNodeId, connectionId.inPortIndex);
        eraseFromPortMap(_outConnectionsByPort, connectionId.outNodeId, connectionId.outPortIndex);
    }

private:
    ConnectionSet _connectivity;
    std::unordered_map<NodeId, ConnectionSet> _nodeConnections;
    std::unordered_map<NodeId, ConnectionsByPort> _inConnectionsByPort;
    std::unordered_map<NodeId, ConnectionsByPort> _outConnectionsByPort;
};

} // namespace QtNodes
