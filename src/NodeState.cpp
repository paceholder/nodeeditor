#include "NodeState.hpp"

#include "NodeGraphicsObject.hpp"


namespace QtNodes
{

NodeState::
NodeState(NodeGraphicsObject & ngo)
  : _ngo(ngo)
  , _hovered(false)
  , _reaction(NOT_REACTING)
  , _reactingPortType(PortType::None)
  //, _locked(false)
  , _resizing(false)
{}


//std::vector<NodeState::ConnectionPtrSet> const &
//NodeState::
//getEntries(PortType portType) const
//{
//if (portType == PortType::In)
//return _inConnections;
//else
//return _outConnections;
//}


//std::vector<NodeState::ConnectionPtrSet> &
//NodeState::
//getEntries(PortType portType)
//{
//if (portType == PortType::In)
//return _inConnections;
//else
//return _outConnections;
//}


//NodeState::ConnectionPtrSet
//NodeState::
//connections(PortType portType, PortIndex portIndex) const
//{
//auto const &connections = getEntries(portType);

//return connections[portIndex];
//}


//void
//NodeState::
//setConnection(PortType portType,
//PortIndex portIndex,
//Connection& connection)
//{
//auto &connections = getEntries(portType);

//connections.at(portIndex).insert(std::make_pair(connection.id(),
//&connection));
//}


//void
//NodeState::
//eraseConnection(PortType portType,
//PortIndex portIndex,
//QUuid id)
//{
//getEntries(portType)[portIndex].erase(id);
//}


void
NodeState::
setResizing(bool resizing)
{
  _resizing = resizing;
}


bool
NodeState::
resizing() const
{
  return _resizing;
}


//void
//NodeState::
//lock(bool locked)
//{
//_locked = locked;

//setFlag(QGraphicsItem::ItemIsMovable,    !locked);
//setFlag(QGraphicsItem::ItemIsFocusable,  !locked);
//setFlag(QGraphicsItem::ItemIsSelectable, !locked);
//}


NodeState::ReactToConnectionState
NodeState::
reaction() const
{
  return _reaction;
}


PortType
NodeState::
reactingPortType() const
{
  return _reactingPortType;
}


NodeDataType
NodeState::
reactingDataType() const
{
  return _reactingDataType;
}


void
NodeState::
setReaction(ReactToConnectionState reaction,
            PortType reactingPortType,
            NodeDataType reactingDataType)
{
  _reaction = reaction;

  _reactingPortType = reactingPortType;

  _reactingDataType = std::move(reactingDataType);
}


bool
NodeState::
isReacting() const
{
  return _reaction == REACTING;
}


void
NodeState::
reactToPossibleConnection(PortType reactingPortType,
                          NodeDataType const & reactingDataType,
                          QPointF const & scenePoint)
{
  QTransform const t = _ngo.sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  _draggingPos = p;

  setReaction(NodeState::REACTING,
              reactingPortType,
              reactingDataType);

  _ngo.update();

}


void
NodeState::
resetReactionToConnection()
{
  setReaction(NodeState::NOT_REACTING);

  _ngo.update();
}


}
