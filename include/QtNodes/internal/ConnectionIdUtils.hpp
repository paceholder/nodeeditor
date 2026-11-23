#pragma once

#include "Definitions.hpp"

#include <QJsonObject>

#include <iostream>
#include <limits>
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
    connJson["inNodeId"] = static_cast<qint64>(connId.inNodeId);
    connJson["inPortIndex"] = static_cast<qint64>(connId.inPortIndex);

    return connJson;
}

// The invalid value is the same for all ids. jsonValueToUInt32() returns it on failure.
static_assert(InvalidNodeId == InvalidGroupId && InvalidNodeId == InvalidPortIndex
                  && InvalidNodeId == std::numeric_limits<quint32>::max(),
              "InvalidNodeId, InvalidGroupId and InvalidPortIndex must be the same value");

/**
 * Reads an unsigned 32-bit id from JSON.
 * Returns the maximum value (InvalidNodeId, InvalidGroupId, InvalidPortIndex) when the value is
 * not a number in the unsigned 32-bit range.
 * QJsonValue::toInt() returns the default value for numbers above the int range.
 * This function accepts the whole unsigned range.
 */
inline quint32 jsonValueToUInt32(QJsonValue const &value)
{
    constexpr quint32 invalid = std::numeric_limits<quint32>::max();

    if (!value.isDouble())
        return invalid;

    double const number = value.toDouble();
    if (number < 0.0 || number > static_cast<double>(invalid))
        return invalid;

    return static_cast<quint32>(number);
}

inline ConnectionId fromJson(QJsonObject const &connJson)
{
    // Support both "inNodeId" (correct) and "intNodeId" (legacy typo) for backward compatibility
    NodeId inNodeId = InvalidNodeId;
    if (connJson.contains("inNodeId")) {
        inNodeId = jsonValueToUInt32(connJson["inNodeId"]);
    } else if (connJson.contains("intNodeId")) {
        inNodeId = jsonValueToUInt32(connJson["intNodeId"]);
    }

    ConnectionId connId{jsonValueToUInt32(connJson["outNodeId"]),
                        jsonValueToUInt32(connJson["outPortIndex"]),
                        inNodeId,
                        jsonValueToUInt32(connJson["inPortIndex"])};

    return connId;
}

} // namespace QtNodes
