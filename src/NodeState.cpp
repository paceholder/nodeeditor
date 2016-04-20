#include "NodeState.hpp"

#include "NodeDataModel.hpp"

NodeState::
NodeState(std::unique_ptr<NodeDataModel> const &model)
  : _sources(model->nSlots(PortType::OUT), QUuid())
  , _sinks(model->nSlots(PortType::IN), QUuid())
  , _reaction(NOT_REACTING)
{}


std::vector<QUuid> const&
NodeState::
getEntries(PortType portType) const
{
  Q_ASSERT(portType != PortType::NONE);

  if (portType == PortType::OUT)
    return _sources;
  else
    return _sinks;
}


QUuid const
NodeState::
connectionID(PortType portType, size_t nEntry) const
{
  std::vector<QUuid> const& entries =
    getEntries(portType);

  if (nEntry >= entries.size())
    return QUuid();

  return entries[nEntry];
}


void
NodeState::
setConnectionId(PortType portType, size_t nEntry, QUuid id)
{
  std::vector<QUuid> const& entries =
    const_cast<NodeState const&>(*this).getEntries(portType);

  const_cast<std::vector<QUuid>&>(entries)[nEntry] = id;
}


NodeState::ReactToConnectionState
NodeState::
reaction() const
{ return _reaction; }

void
NodeState::
setReaction(ReactToConnectionState reaction)
{ _reaction = reaction; }

bool
NodeState::
isReacting() const
{ return _reaction == REACTING; }
