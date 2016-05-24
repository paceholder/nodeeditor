#pragma once

#include <memory>
#include <vector>

#include <QtCore/QUuid>

#include "PortType.hpp"

class Connection;
class NodeDataModel;

/// Contains vectors of connected input and output connections.
/// Stores bool for reacting on hovering connections
class NodeState
{
public:
  enum ReactToConnectionState
  {
    REACTING,
    NOT_REACTING
  };

public:

  NodeState(std::unique_ptr<NodeDataModel> const &model);

public:

  /// Returns vector of connections ID.
  /// Some of them can be empty (null)
  std::vector<std::weak_ptr<Connection> > const& getEntries(PortType portType) const;
  std::vector<std::weak_ptr<Connection> > & getEntries(PortType portType);

  std::shared_ptr<Connection> connection(PortType portType,
                                         PortIndex portIndex) const;

  void setConnection(PortType portType,
                     PortIndex portIndex,
                     std::shared_ptr<Connection> connection);

  ReactToConnectionState reaction() const;

  void setReaction(ReactToConnectionState reaction);

  bool isReacting() const;

private:

  //std::vector<QUuid> _sources;
  //std::vector<QUuid> _sinks;

  std::vector < std::weak_ptr < Connection > > _inConnections;
  std::vector < std::weak_ptr < Connection > > _outConnections;

  ReactToConnectionState _reaction;
};
