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


bool dfs(AbstractGraphModel &model, NodeId currentNode, NodeId targetNode, std::unordered_map<NodeId, bool>& visited) {

    if (currentNode == targetNode) {
        // Target node reached, cycle found
        return true;
    }
    if(visited[currentNode]) {
        return false;
    }
    visited[currentNode] = true;

    unsigned int nOutPorts = model.nodeData<PortCount>(currentNode, NodeRole::OutPortCount);
    for (PortIndex index = 0; index < nOutPorts; ++index) {
        auto const &outConnectionIds = model.connections(currentNode, PortType::Out, index);
        for (auto& connection : outConnectionIds) {
            NodeId neighbour  = connection.inNodeId;
            if (!visited[neighbour]) {
                if (dfs(model, neighbour, targetNode, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool NodeConnectionInteraction::introducesCycle(AbstractGraphModel &model, NodeId sourceNode, NodeId targetNode) const {
    // Mark all nodes as not visited
    std::unordered_map<NodeId, bool> visited;
    for (auto& id : model.allNodeIds()) {
        visited[id] = false;
    }

    // Perform DFS from the target node
    return dfs(model, sourceNode, targetNode, visited);
}


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

    AbstractGraphModel &model = _ngo.nodeScene()->graphModel();

    // 3. Forbid connections that introduce cycles
    if(introducesCycle(model, _ngo.nodeId(), connectedNodeId)) {
        return false;
    }

    // 4. Connection loose end is above the node port.

    QPointF connectionPoint = _cgo.sceneTransform().map(_cgo.endPoint(requiredPort));

    *portIndex = nodePortIndexUnderScenePoint(requiredPort, connectionPoint);

    if (*portIndex == InvalidPortIndex) {
        return false;
    }

    // 5. Model allows connection.


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


    // 2. Remove existing connections to the port
    // TODO: change to a different node id if the connection is created the other way around
    AbstractGraphModel &model = _ngo.nodeScene()->graphModel();
    auto const connected = model.connections(_ngo.nodeId(), PortType::In, targetPortIndex);
    if(!connected.empty()) {
        for(auto conId : connected) {
            _scene.undoStack().push(new DisconnectCommand(&_scene, conId));
        }
    }

    // 3. Create new connection.

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
