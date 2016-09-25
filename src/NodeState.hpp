#pragma once

#include <memory>
#include <vector>

#include <QtCore/QUuid>

#include "PortType.hpp"
#include "NodeData.hpp"

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
  std::vector<std::weak_ptr<Connection> > const&
  getEntries(PortType portType) const;

  std::vector<std::weak_ptr<Connection> > &
  getEntries(PortType portType);

  std::shared_ptr<Connection>
  connection(PortType portType,
             PortIndex portIndex) const;

  void
  setConnection(PortType portType,
                PortIndex portIndex,
                std::shared_ptr<Connection> connection);

  ReactToConnectionState
  reaction() const;

  PortType
  reactingPortType() const;

  NodeDataType
  reactingDataType() const;

  void
  setReaction(ReactToConnectionState reaction,
              PortType reactingPortType = PortType::None,

              NodeDataType reactingDataType =
                NodeDataType());

  bool
  isReacting() const;

  void
  setResizing(bool resizing);

  bool
  resizing() const;

private:

  std::vector<std::weak_ptr<Connection> > _inConnections;
  std::vector<std::weak_ptr<Connection> > _outConnections;

  ReactToConnectionState _reaction;
  PortType     _reactingPortType;
  NodeDataType _reactingDataType;

  bool _resizing;
};
