#include "NodeState.hpp"

#include "NodeDataModel.hpp"

NodeState::
NodeState(unsigned int nSources,
          unsigned int nSinks)
  : _sources(nSources, QUuid())
  , _sinks(nSinks, QUuid())
  , _reaction(NOT_REACTING)
{}

NodeState::
NodeState(std::unique_ptr<NodeDataModel> const &model)
  : _sources(model->nSlots(EndType::SOURCE), QUuid())
  , _sinks(model->nSlots(EndType::SOURCE), QUuid())
  , _reaction(NOT_REACTING)
{}


std::vector<QUuid> const&
NodeState::
getEntries(EndType endType) const
{
  Q_ASSERT(endType != EndType::NONE);

  if (endType == EndType::SOURCE)
    return _sources;
  else
    return _sinks;
}


QUuid const
NodeState::
connectionID(EndType endType, size_t nEntry) const
{
  std::vector<QUuid> const& entries =
    getEntries(endType);

  if (nEntry >= entries.size())
    return QUuid();

  return entries[nEntry];
}


void
NodeState::
setConnectionId(EndType endType, size_t nEntry, QUuid id)
{
  std::vector<QUuid> const& entries =
    const_cast<NodeState const&>(*this).getEntries(endType);

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
