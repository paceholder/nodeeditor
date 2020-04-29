#include "NodeState.hpp"

#include "NodeDataModel.hpp"

#include "Connection.hpp"

using QtNodes::NodeState;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::Connection;

NodeState::
NodeState(std::unique_ptr<NodeDataModel> const &model)
  : _inConnections(model->nPorts(PortType::In))
  , _outConnections(model->nPorts(PortType::Out))
  , _reaction(NOT_REACTING)
  , _reactingPortType(PortType::None)
  , _resizing(false)
{}


std::vector<NodeState::ConnectionPtrSet> const &
NodeState::
getEntries(PortType portType) const
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}


std::vector<NodeState::ConnectionPtrSet> &
NodeState::
getEntries(PortType portType)
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}


NodeState::ConnectionPtrSet
NodeState::
connections(PortType portType, PortIndex portIndex) const
{
  auto const &connections = getEntries(portType);

  return connections[portIndex];
}


void
NodeState::
setConnection(PortType portType,
              PortIndex portIndex,
              Connection& connection)
{
  auto &connections = getEntries(portType);

  connections.at(portIndex).insert(std::make_pair(connection.id(),
                                               &connection));
}


void
NodeState::
eraseConnection(PortType portType,
                PortIndex portIndex,
                QUuid id)
{
  getEntries(portType)[portIndex].erase(id);
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

void
NodeState::
insertPort(const PortType& portType,
           const size_t index)
{
  auto& ports = getEntries(portType);
  assert(index <= ports.size());
  ports.emplace(std::next(ports.begin(), index));
  updateConnectionIndices(portType, index + 1);
}

void
NodeState::
erasePort(const PortType portType,
          const size_t index)
{
  auto& ports = getEntries(portType);
  assert(index < ports.size());
  ports.erase(std::next(ports.begin(), index));
  updateConnectionIndices(portType, index);
}

void
NodeState::
updateConnectionIndices(const PortType portType,
                        const size_t index)
{
  auto& ports = getEntries(portType);
  for (size_t i = index; i < ports.size(); i++)
  {
    for (const auto& entry : ports[i])
    {
      entry.second->setPortIndex(portType, i);
    }
  }
}
