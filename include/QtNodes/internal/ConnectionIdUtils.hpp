#pragma once

#include "Definitions.hpp"
#include "SerializationValidation.hpp"

#include <QJsonObject>
#include <QtCore/QtGlobal>

#include <iostream>
#include <string>

namespace QtNodes {

inline NodeId getNodeId(PortType portType, ConnectionId const &connectionId) noexcept
{
    NodeId id = InvalidNodeId;

    if (portType == PortType::Out) {
        id = connectionId.outNodeId;
    } else if (portType == PortType::In) {
        id = connectionId.inNodeId;
    }

    return id;
}

inline PortIndex getPortIndex(PortType portType, ConnectionId const &connectionId) noexcept
{
    PortIndex index = InvalidPortIndex;

    if (portType == PortType::Out) {
        index = connectionId.outPortIndex;
    } else if (portType == PortType::In) {
        index = connectionId.inPortIndex;
    }

    return index;
}

inline PortType oppositePort(PortType port) noexcept
{
    switch (port) {
    case PortType::In:
        return PortType::Out;
    case PortType::Out:
        return PortType::In;
    default:
        return PortType::None;
    }
}

inline bool isPortIndexValid(PortIndex index) noexcept
{
    return index != InvalidPortIndex;
}

inline bool isPortTypeValid(PortType portType) noexcept
{
    return portType != PortType::None;
}

/**
 * Creates a connection Id instance filled just on one side.
 */
inline ConnectionId makeIncompleteConnectionId(NodeId const connectedNodeId,
                                               PortType const connectedPort,
                                               PortIndex const connectedPortIndex)
{
    return (connectedPort == PortType::In)
               ? ConnectionId{InvalidNodeId, InvalidPortIndex, connectedNodeId, connectedPortIndex}
               : ConnectionId{connectedNodeId, connectedPortIndex, InvalidNodeId, InvalidPortIndex};
}

/**
 * Turns a full connection Id into an incomplete one by removing the
 * data on the given side
 */
inline ConnectionId makeIncompleteConnectionId(ConnectionId connectionId,
                                               PortType const portToDisconnect)
{
    if (portToDisconnect == PortType::Out) {
        connectionId.outNodeId = InvalidNodeId;
        connectionId.outPortIndex = InvalidPortIndex;
    } else {
        connectionId.inNodeId = InvalidNodeId;
        connectionId.inPortIndex = InvalidPortIndex;
    }

    return connectionId;
}

inline ConnectionId makeCompleteConnectionId(ConnectionId incompleteConnectionId,
                                             NodeId const nodeId,
                                             PortIndex const portIndex)
{
    if (incompleteConnectionId.outNodeId == InvalidNodeId) {
        incompleteConnectionId.outNodeId = nodeId;
        incompleteConnectionId.outPortIndex = portIndex;
    } else {
        incompleteConnectionId.inNodeId = nodeId;
        incompleteConnectionId.inPortIndex = portIndex;
    }

    return incompleteConnectionId;
}

inline std::ostream &operator<<(std::ostream &ostr, ConnectionId const connectionId)
{
    ostr << "(" << connectionId.outNodeId << ", "
         << (isPortIndexValid(connectionId.outPortIndex) ? std::to_string(connectionId.outPortIndex)
                                                         : "INVALID")
         << ", " << connectionId.inNodeId << ", "
         << (isPortIndexValid(connectionId.inPortIndex) ? std::to_string(connectionId.inPortIndex)
                                                        : "INVALID")
         << ")" << std::endl;

    return ostr;
}

inline QJsonObject toJson(ConnectionId const &connId)
{
    QJsonObject connJson;

    connJson["outNodeId"] = static_cast<qint64>(connId.outNodeId);
    connJson["outPortIndex"] = static_cast<qint64>(connId.outPortIndex);
    connJson["inNodeId"] = static_cast<qint64>(connId.inNodeId);
    connJson["inPortIndex"] = static_cast<qint64>(connId.inPortIndex);

    return connJson;
}

inline bool tryFromJson(QJsonObject const &connJson, ConnectionId &connId)
{
    NodeId outNodeId = InvalidNodeId;
    NodeId inNodeId = InvalidNodeId;
    PortIndex outPortIndex = InvalidPortIndex;
    PortIndex inPortIndex = InvalidPortIndex;

    if (!detail::read_node_id(connJson["outNodeId"], outNodeId)
        || !detail::read_port_index(connJson["outPortIndex"], outPortIndex)
        || !detail::read_port_index(connJson["inPortIndex"], inPortIndex)) {
        return false;
    }

    // Support both "inNodeId" (correct) and "intNodeId" (legacy typo)
    if (!detail::read_node_id(connJson["inNodeId"], inNodeId)
        && !detail::read_node_id(connJson["intNodeId"], inNodeId)) {
        return false;
    }

    connId = ConnectionId{outNodeId, outPortIndex, inNodeId, inPortIndex};
    return true;
}

inline ConnectionId fromJson(QJsonObject const &connJson)
{
    ConnectionId connId{InvalidNodeId, InvalidPortIndex, InvalidNodeId, InvalidPortIndex};

    bool const ok = tryFromJson(connJson, connId);
    Q_ASSERT(ok);

    return connId;
}

inline NodeRole portCountRole(PortType portType) noexcept
{
    return (portType == PortType::Out) ? NodeRole::OutPortCount : NodeRole::InPortCount;
}

} // namespace QtNodes
