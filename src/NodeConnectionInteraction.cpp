#include "NodeConnectionInteraction.hpp"

#include "AbstractNodeGeometry.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeGraphicsObject.hpp"
#include "UndoCommands.hpp"

#include <QtCore/QDebug>

#include <QUndoStack>

namespace QtNodes {

NodeConnectionInteraction::NodeConnectionInteraction(NodeGraphicsObject &ngo,
                                                     ConnectionGraphicsObject &cgo,
                                                     BasicGraphicsScene &scene)
    : _ngo(ngo)
    , _cgo(cgo)
    , _scene(scene)
{}

bool NodeConnectionInteraction::canConnect(PortIndex *portIndex) const
{
    // 1. Connection requires a port.

    PortType requiredPort = _cgo.connectionState().requiredPort();

    if (requiredPort == PortType::None) {
        return false;
    }

    NodeId connectedNodeId = getNodeId(oppositePort(requiredPort), _cgo.connectionId());

    // 2. Forbid connecting the node to itself.

    if (_ngo.nodeId() == connectedNodeId)
        return false;

    // 3. Connection loose end is above the node port.

    QPointF connectionPoint = _cgo.sceneTransform().map(_cgo.endPoint(requiredPort));

    *portIndex = nodePortIndexUnderScenePoint(requiredPort, connectionPoint);

    if (*portIndex == InvalidPortIndex) {
        return false;
    }

    // 4. Model allows connection.

    AbstractGraphModel &model = _ngo.nodeScene()->graphModel();

    ConnectionId connectionId = makeCompleteConnectionId(_cgo.connectionId(), // incomplete
                                                         _ngo.nodeId(),       // missing node id
                                                         *portIndex);         // missing port index

    return model.connectionPossible(connectionId);
}

bool NodeConnectionInteraction::tryConnect() const
{
    // 1. Check conditions from 'canConnect'.

    PortIndex targetPortIndex = InvalidPortIndex;
    if (!canConnect(&targetPortIndex)) {
        return false;
    }

    // 2. Create new connection.

    ConnectionId incompleteConnectionId = _cgo.connectionId();

    ConnectionId newConnectionId = makeCompleteConnectionId(incompleteConnectionId,
                                                            _ngo.nodeId(),
                                                            targetPortIndex);

    _ngo.nodeScene()->resetDraftConnection();

    _ngo.nodeScene()->undoStack().push(new ConnectCommand(_ngo.nodeScene(), newConnectionId));

    return true;
}

bool NodeConnectionInteraction::disconnect(PortType portToDisconnect) const
{
    ConnectionId connectionId = _cgo.connectionId();

    _scene.undoStack().push(new DisconnectCommand(&_scene, connectionId));

    AbstractNodeGeometry &geometry = _scene.nodeGeometry();

    QPointF scenePos = geometry.portScenePosition(_ngo.nodeId(),
                                                  portToDisconnect,
                                                  getPortIndex(portToDisconnect, connectionId),
                                                  _ngo.sceneTransform());

    // Converted to "draft" connection with the new incomplete id.
    ConnectionId incompleteConnectionId = makeIncompleteConnectionId(connectionId, portToDisconnect);

    // Grabs the mouse
    auto const &draftConnection = _scene.makeDraftConnection(incompleteConnectionId);

    QPointF const looseEndPos = draftConnection->mapFromScene(scenePos);
    draftConnection->setEndPoint(portToDisconnect, looseEndPos);

    // Repaint connection points.
    NodeId connectedNodeId = getNodeId(oppositePort(portToDisconnect), connectionId);
    _scene.nodeGraphicsObject(connectedNodeId)->update();

    NodeId disconnectedNodeId = getNodeId(portToDisconnect, connectionId);
    _scene.nodeGraphicsObject(disconnectedNodeId)->update();

    return true;
}

// ------------------ util functions below

PortType NodeConnectionInteraction::connectionRequiredPort() const
{
    auto const &state = _cgo.connectionState();

    return state.requiredPort();
}

QPointF NodeConnectionInteraction::nodePortScenePosition(PortType portType,
                                                         PortIndex portIndex) const
{
    AbstractNodeGeometry &geometry = _scene.nodeGeometry();

    QPointF p = geometry.portScenePosition(_ngo.nodeId(),
                                           portType,
                                           portIndex,
                                           _ngo.sceneTransform());

    return p;
}

PortIndex NodeConnectionInteraction::nodePortIndexUnderScenePoint(PortType portType,
                                                                  QPointF const &scenePoint) const
{
    AbstractNodeGeometry &geometry = _scene.nodeGeometry();

    QTransform sceneTransform = _ngo.sceneTransform();

    QPointF nodePoint = sceneTransform.inverted().map(scenePoint);

    return geometry.checkPortHit(_ngo.nodeId(), portType, nodePoint);
}

} // namespace QtNodes
