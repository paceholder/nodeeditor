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

class Connection;
class NodeDataModel;

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

  NodeState(std::unique_ptr<NodeDataModel> const &model);

public:

  using ConnectionPtrSet =
          std::unordered_map<QUuid, Connection*>;

  /// Returns vector of connections ID.
  /// Some of them can be empty (null)
  std::vector<ConnectionPtrSet> const&
  getEntries(PortType) const;

  std::vector<ConnectionPtrSet> &
  getEntries(PortType);

  ConnectionPtrSet
  connections(PortType portType, PortIndex portIndex) const;

  void
  setConnection(PortType portType,
                PortIndex portIndex,
                Connection& connection);

  void
  eraseConnection(PortType portType,
                  PortIndex portIndex,
                  QUuid id);

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

  /**
   * @brief Adds a new input/output port at the desired index.
   * @param portType Type of port (input or output)
   * @param index Index at which the new port will be placed
   */
  void
  insertPort(const PortType& portType, const size_t index);

  /**
   * @brief Removes the input/output port at the given index.
   * @param portType Type of port (input or output)
   * @param index Index at which the port will be removed
   */
  void
  erasePort(const PortType portType, const size_t index);

private:

  std::vector<ConnectionPtrSet> _inConnections;
  std::vector<ConnectionPtrSet> _outConnections;

  ReactToConnectionState _reaction;
  PortType     _reactingPortType;
  NodeDataType _reactingDataType;

  bool _resizing;
};
}
