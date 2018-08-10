#pragma once

#include "Node.hpp"
#include "Connection.hpp"

namespace QtNodes
{

/// Class performs various operations on the Node and Connection pair.
/// An instance should be created on the stack and destroyed when
/// the operation is completed
class NodeConnectionInteraction
{
public:
  NodeConnectionInteraction(NodeIndex const& node,
                            ConnectionGraphicsObject& connection);

  /// Can connect when following conditions are met:
  /// 1) Connection 'requires' a port
  /// 2) Connection's vacant end is above the node port
  /// 3) Node port is vacant
  /// 4) Connection type equals node port type, or there is a registered type conversion that can translate between the two
  bool canConnect(PortIndex & portIndex, bool& converted) const;

  /// 1)   Check conditions from 'canConnect'
  /// 1.5) If the connection is possible but a type conversion is needed, add a converter node to the scene, and connect it properly
  /// 2)   Assign node to required port in Connection
  /// 3)   Assign Connection to empty port in NodeState
  /// 4)   Adjust Connection geometry
  /// 5)   Poke model to initiate data transfer
  bool tryConnect() const;

private:

  PortType connectionRequiredPort() const;

  QPointF connectionEndScenePosition(PortType) const;

  QPointF nodePortScenePosition(PortType portType,
                                PortIndex portIndex) const;

  PortIndex nodePortIndexUnderScenePoint(PortType portType,
                                         QPointF const &p) const;

  bool nodePortIsEmpty(PortType portType, PortIndex portIndex) const;

private:

  NodeIndex _node;

  ConnectionGraphicsObject* _connection;
};
}
