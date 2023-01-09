#pragma once

#include "Definitions.hpp"

#include <QJsonObject>

#include <iostream>
#include <string>

namespace QtNodes {

inline PortIndex getNodeId(PortType portType, ConnectionId connectionId)
{
    NodeId id = InvalidNodeId;

    if (portType == PortType::Out) {
        id = connectionId.outNodeId;
    } else if (portType == PortType::In) {
        id = connectionId.inNodeId;
    }

    return id;
}

inline PortIndex getPortIndex(PortType portType, ConnectionId connectionId)
{
    PortIndex index = InvalidPortIndex;

    if (portType == PortType::Out) {
        index = connectionId.outPortIndex;
    } else if (portType == PortType::In) {
        index = connectionId.inPortIndex;
    }

    return index;
}

inline PortType oppositePort(PortType port)
{
    PortType result = PortType::None;

    switch (port) {
    case PortType::In:
        result = PortType::Out;
        break;

    case PortType::Out:
        result = PortType::In;
        break;

    case PortType::None:
        result = PortType::None;
        break;

    default:
        break;
    }
    return result;
}

inline bool isPortIndexValid(PortIndex index)
{
    return index != InvalidPortIndex;
}

inline bool isPortTypeValid(PortType portType)
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
    connJson["intNodeId"] = static_cast<qint64>(connId.inNodeId);
    connJson["inPortIndex"] = static_cast<qint64>(connId.inPortIndex);

    return connJson;
}

inline ConnectionId fromJson(QJsonObject const &connJson)
{
    ConnectionId connId{static_cast<NodeId>(connJson["outNodeId"].toInt(InvalidNodeId)),
                        static_cast<PortIndex>(connJson["outPortIndex"].toInt(InvalidPortIndex)),
                        static_cast<NodeId>(connJson["intNodeId"].toInt(InvalidNodeId)),
                        static_cast<PortIndex>(connJson["inPortIndex"].toInt(InvalidPortIndex))};

    return connId;
}

} // namespace QtNodes
