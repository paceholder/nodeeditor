#include "NodeState.hpp"

#include "NodeDataModel.hpp"

NodeState::
NodeState(std::unique_ptr<NodeDataModel> const &model)
  : _outConnections(model->nPorts(PortType::Out))
  , _inConnections(model->nPorts(PortType::In))
  , _reaction(NOT_REACTING)
  , _reactingPortType(PortType::None)
  , _resizing(false)
{}

std::vector<std::weak_ptr<Connection> > const&
NodeState::
getEntries(PortType portType) const
{
  Q_ASSERT(portType != PortType::None);

  if (portType == PortType::Out)
    return _outConnections;
  else
    return _inConnections;
}


std::vector<std::weak_ptr<Connection> > &
NodeState::
getEntries(PortType portType)
{
  Q_ASSERT(portType != PortType::None);

  if (portType == PortType::Out)
    return _outConnections;
  else
    return _inConnections;
}


std::shared_ptr<Connection>
NodeState::
connection(PortType portType, PortIndex portIndex) const
{
  auto const &connections = getEntries(portType);

  return connections[portIndex].lock();
}


void
NodeState::
setConnection(PortType portType,
              PortIndex portIndex,
              std::shared_ptr<Connection> connection)
{
  auto &connections = getEntries(portType);

  connections[portIndex] = connection;
}


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

  _reactingDataType = reactingDataType;
}


bool
NodeState::
isReacting() const
{
  return _reaction == REACTING;
}


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
