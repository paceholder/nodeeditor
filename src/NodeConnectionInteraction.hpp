#pragma once

#include <memory>

#include <QtCore/QPointF>

#include "Definitions.hpp"

namespace QtNodes
{

class ConnectionGraphicsObject;
class NodeGraphicsObject;
class BasicGraphicsScene;

/// Class performs various operations on the Node and Connection pair.
/// An instance should be created on the stack and destroyed when
/// the operation is completed
class NodeConnectionInteraction
{
public:
  NodeConnectionInteraction(NodeGraphicsObject & ngo,
                            ConnectionGraphicsObject & cgo,
                            BasicGraphicsScene & scene);

  /// Can connect when following conditions are met:
  /// 1) Connection 'requires' a port
  /// 2) Connection's vacant end is above the node port
  /// 3) Node port is vacant
  /// 4) Connection type equals node port type, or there is a registered type conversion that can translate between the two
  //bool canConnect(PortIndex & portIndex,
  //TypeConverter & converter) const;

  bool canConnect(PortIndex * portIndex) const;

  /// 1)   Check conditions from 'canConnect'
  /// 2)   Assign node to required port in Connection
  /// 3)   Assign Connection to empty port in NodeState
  /// 4)   Adjust Connection geometry
  /// 5)   Poke model to initiate data transfer
  bool tryConnect() const;


  /// 1) Node and Connection should be already connected
  /// 2) If so, clear Connection entry in the NodeState
  /// 3) Propagate invalid data to IN node
  /// 4) Set Connection end to 'requiring a port'
  bool disconnect(PortType portToDisconnect) const;

private:

  PortType connectionRequiredPort() const;

  QPointF connectionEndScenePosition(PortType) const;

  QPointF nodePortScenePosition(PortType  portType,
                                PortIndex portIndex) const;

  PortIndex nodePortIndexUnderScenePoint(PortType portType,
                                         QPointF const &p) const;

  bool nodePortIsEmpty(PortType portType, PortIndex portIndex) const;

private:

  NodeGraphicsObject & _ngo;

  ConnectionGraphicsObject & _cgo;

  BasicGraphicsScene & _scene;
};

}
