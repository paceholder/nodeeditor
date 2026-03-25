#include "ConnectionState.hpp"

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"

#include <QtCore/QDebug>


namespace QtNodes {

ConnectionState::~ConnectionState() = default;

PortType ConnectionState::requiredPort() const
{
    PortType t = PortType::None;

    if (_cgo.connectionId().outNodeId == InvalidNodeId) {
        t = PortType::Out;
    } else if (_cgo.connectionId().inNodeId == InvalidNodeId) {
        t = PortType::In;
    }

    return t;
}

bool ConnectionState::requiresPort() const
{
    const ConnectionId &id = _cgo.connectionId();
    return id.outNodeId == InvalidNodeId || id.inNodeId == InvalidNodeId;
}

bool ConnectionState::hovered() const noexcept
{
    return _hovered;
}

void ConnectionState::setHovered(bool hovered) noexcept
{
    _hovered = hovered;
}

void ConnectionState::setLastHoveredNode(NodeId const nodeId)
{
    _lastHoveredNode = nodeId;
}

NodeId ConnectionState::lastHoveredNode() const noexcept
{
    return _lastHoveredNode;
}

void ConnectionState::resetLastHoveredNode()
{
    if (_lastHoveredNode != InvalidNodeId) {
        if (auto *scene = _cgo.nodeScene()) {
            if (auto *ngo = scene->nodeGraphicsObject(_lastHoveredNode)) {
                ngo->update();
            }
        }
    }

    _lastHoveredNode = InvalidNodeId;
}

} // namespace QtNodes
