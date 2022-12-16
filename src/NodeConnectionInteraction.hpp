#pragma once

#include <memory>

#include <QtCore/QPointF>

#include "Definitions.hpp"

namespace QtNodes {

class ConnectionGraphicsObject;
class NodeGraphicsObject;
class BasicGraphicsScene;

/// Class wraps conecting and disconnecting checks.
/**
 * An instance should be created on the stack and destroyed
 * automatically when the operation is completed
 */
class NodeConnectionInteraction
{
public:
    NodeConnectionInteraction(NodeGraphicsObject &ngo,
                              ConnectionGraphicsObject &cgo,
                              BasicGraphicsScene &scene);

    /**
   * Can connect when following conditions are met:
   * 1. Connection 'requires' a port.
   * 2. Connection loose end is above the node port.
   * 3. Source and target `nodeId`s are different.
   * 4. GraphModel permits connection.
   */
    bool canConnect(PortIndex *portIndex) const;

    /// Creates a new connectino if possible.
    /**
   * 1. Check conditions from 'canConnect'.
   * 2. Creates new connection with `GraphModel::addConnection`.
   * 3. Adjust connection geometry.
   */
    bool tryConnect() const;

    /**
   * 1. Delete connection with `GraphModel::deleteConnection`.
   * 2. Create a "draft" connection with incomplete `ConnectionId`.
   * 3. Repaint both previously connected nodes.
   */
    bool disconnect(PortType portToDisconnect) const;

private:
    PortType connectionRequiredPort() const;

    QPointF connectionEndScenePosition(PortType) const;

    QPointF nodePortScenePosition(PortType portType, PortIndex portIndex) const;

    PortIndex nodePortIndexUnderScenePoint(PortType portType, QPointF const &p) const;

private:
    NodeGraphicsObject &_ngo;

    ConnectionGraphicsObject &_cgo;

    BasicGraphicsScene &_scene;
};

} // namespace QtNodes
