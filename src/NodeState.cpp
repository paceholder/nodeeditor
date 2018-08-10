#include "NodeState.hpp"

#include "NodeDataModel.hpp"
#include "NodeIndex.hpp"
#include "FlowSceneModel.hpp"
#include "QUuidStdHash.hpp"

#include "Connection.hpp"

using QtNodes::NodeState;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::Connection;

NodeState::
NodeState(NodeIndex const &index)
  : _inConnections(index.model()->nodePortCount(index, PortType::In))
  , _outConnections(index.model()->nodePortCount(index, PortType::Out))
  , _reaction(NOT_REACTING)
  , _reactingPortType(PortType::None)
  , _resizing(false)
{}


std::vector<NodeState::ConnectionPtrVec> const &
NodeState::
getEntries(PortType portType) const
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}


std::vector<NodeState::ConnectionPtrVec> &
NodeState::
getEntries(PortType portType)
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}


NodeState::ConnectionPtrVec
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
              ConnectionGraphicsObject& connection)
{
  auto &connections = getEntries(portType);

  connections[portIndex].push_back(&connection);
}


void
NodeState::
eraseConnection(PortType portType,
                PortIndex portIndex,
                 ConnectionGraphicsObject& conn)
{
  auto& ptrSet = getEntries(portType)[portIndex];
  auto iter = std::find(ptrSet.begin(), ptrSet.end(), &conn);
  if (iter != ptrSet.end()) {
    ptrSet.erase(iter);
  }
  
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
