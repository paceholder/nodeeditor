#ifndef NODE_STATE_HPP
#define NODE_STATE_HPP

#include <vector>

#include <QtCore/QUuid>

#include "EndType.hpp"

class NodeState
{

public:
  NodeState(unsigned int nSources,
            unsigned int nSinks)
    : _sources(nSources, QUuid())
    , _sinks(nSinks, QUuid())
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

private:
  std::vector<QUuid> _sources;
  std::vector<QUuid> _sinks;
};

#endif //  NODE_STATE_HPP
