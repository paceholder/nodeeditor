#pragma once

#include <vector>
#include <unordered_map>

#include <QtCore/QUuid>

#include "Export.hpp"

#include "PortType.hpp"
#include "NodeData.hpp"
#include "memory.hpp"

namespace QtNodes
{

class NodeIndex;
class ConnectionGraphicsObject;

/// Contains vectors of connected input and output connections.
/// Stores bool for reacting on hovering connections
class NODE_EDITOR_PUBLIC NodeState
{
public:
  enum ReactToConnectionState
  {
    REACTING,
    NOT_REACTING
  };

public:

  NodeState(NodeIndex const& index);

public:

  using ConnectionPtrVec =
          std::vector<ConnectionGraphicsObject*>;

  /// Returns vector of connections ID.
  /// Some of them can be empty (null)
  std::vector<ConnectionPtrVec> const&
  getEntries(PortType) const;

  std::vector<ConnectionPtrVec> &
  getEntries(PortType);

  ConnectionPtrVec
  connections(PortType portType, PortIndex portIndex) const;

  void
  setConnection(PortType portType,
                PortIndex portIndex,
                ConnectionGraphicsObject& connection);

  void
  eraseConnection(PortType portType,
                  PortIndex portIndex,
                  ConnectionGraphicsObject& connection);

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

  std::vector<ConnectionPtrVec> _inConnections;
  std::vector<ConnectionPtrVec> _outConnections;

  ReactToConnectionState _reaction;
  PortType     _reactingPortType;
  NodeDataType _reactingDataType;

  bool _resizing;
};
}
