#ifndef NODE_STATE_HPP
#define NODE_STATE_HPP

#include <vector>

#include <QtCore/QUuid>

#include "EndType.hpp"

class NodeState
{
public:
  enum ReactToConnectionState
  {
    REACTING,
    NOT_REACTING
  };

public:
  NodeState(unsigned int nSources,
            unsigned int nSinks)
    : _sources(nSources, QUuid())
    , _sinks(nSinks, QUuid())
    , _reaction(NOT_REACTING)
  {}

public:

  std::vector<QUuid> const& getEntries(EndType endType) const
  {
    Q_ASSERT(endType != EndType::NONE);

    if (endType == EndType::SOURCE)
      return _sources;
    else
      return _sinks;
  }

  QUuid const connectionID(EndType endType, size_t nEntry) const
  {
    std::vector<QUuid> const& entries =
      getEntries(endType);

    if (nEntry >= entries.size())
      return QUuid();

    return entries[nEntry];
  }

  void setConnectionId(EndType endType, size_t nEntry, QUuid id)
  {
    std::vector<QUuid> const& entries =
      const_cast<NodeState const&>(*this).getEntries(endType);

    const_cast<std::vector<QUuid>&>(entries)[nEntry] = id;
  }

  ReactToConnectionState reaction() const
  { return _reaction; }

  void setReaction(ReactToConnectionState reaction)
  { _reaction = reaction; }

  bool isReacting() const
  { return _reaction == REACTING; }

private:
  std::vector<QUuid> _sources;
  std::vector<QUuid> _sinks;

  ReactToConnectionState _reaction;
};

#endif //  NODE_STATE_HPP
