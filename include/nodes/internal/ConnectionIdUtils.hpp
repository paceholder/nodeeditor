#pragma once

#include <iostream>

#include "Definitions.hpp"

namespace QtNodes
{

inline
PortIndex
getNodeId(PortType portType, ConnectionId connectionId)
{
  NodeId id = InvalidNodeId;

  if (portType == PortType::Out)
  {
    id = std::get<0>(connectionId);
  }
  else if (portType == PortType::In)
  {
    id = std::get<2>(connectionId);
  }

  return id;
}


inline
PortIndex
getPortIndex(PortType portType, ConnectionId connectionId)
{
  PortIndex index = InvalidPortIndex;

  if (portType == PortType::Out)
  {
    index = std::get<1>(connectionId);
  }
  else if (portType == PortType::In)
  {
    index = std::get<3>(connectionId);
  }

  return index;
}


inline
PortType
oppositePort(PortType port)
{
  PortType result = PortType::None;

  switch (port)
  {
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


inline
bool
isPortIndexValid(PortIndex index)
{
  return index != InvalidPortIndex;
}


inline
bool
isPortTypeValid(PortType portType)
{
  return portType != PortType::None;
}


inline
ConnectionId
makeIncompleteConnectionId(PortType const  connectedPort,
                           NodeId const    connectedNodeId,
                           PortIndex const connectedPortIndex)
{
  return (connectedPort == PortType::In) ?
         std::make_tuple(InvalidNodeId, InvalidPortIndex,
                         connectedNodeId, connectedPortIndex) :
         std::make_tuple(connectedNodeId, connectedPortIndex,
                         InvalidNodeId, InvalidPortIndex);
}


inline
ConnectionId
makeIncompleteConnectionId(ConnectionId   connectionId,
                           PortType const portToDisconnect)
{
  if (portToDisconnect == PortType::Out)
  {
    std::get<0>(connectionId) = InvalidNodeId;
    std::get<1>(connectionId) = InvalidPortIndex;
  }
  else
  {
    std::get<2>(connectionId) = InvalidNodeId;
    std::get<3>(connectionId) = InvalidPortIndex;
  }

  return connectionId;
}


inline
ConnectionId
makeCompleteConnectionId(ConnectionId    incompleteConnectionId,
                         NodeId const    nodeId,
                         PortIndex const portIndex)
{
  if (std::get<0>(incompleteConnectionId) == InvalidNodeId)
  {
    std::get<0>(incompleteConnectionId) = nodeId;
    std::get<1>(incompleteConnectionId) = portIndex;
  }
  else
  {
    std::get<2>(incompleteConnectionId) = nodeId;
    std::get<3>(incompleteConnectionId) = portIndex;
  }

  return incompleteConnectionId;
}




inline
std::ostream &
operator<<(std::ostream & ostr, ConnectionId const connectionId)
{
  ostr << "(" << std::get<0>(connectionId)
       << ", " << (isPortIndexValid(std::get<1>(connectionId)) ?  std::to_string(std::get<1>(connectionId)) : "INVALID")
       << ", " << std::get<2>(connectionId)
       << ", " << (isPortIndexValid(std::get<3>(connectionId)) ?  std::to_string(std::get<3>(connectionId)) : "INVALID")
       << ")" << std::endl;

  return ostr;
}


}
