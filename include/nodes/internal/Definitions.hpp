#pragma once

#include <limits>

#include <QtCore/QMetaObject>

/**
 * @file
 * Important definitions used throughout the library.
 */

namespace QtNodes
{
Q_NAMESPACE


enum class NodeRole
{
  Type             = 0,
  Position         = 1,
  Size             = 2,
  CaptionVisible   = 3,
  Caption          = 4,
  Style            = 5,
  NumberOfInPorts  = 6,
  NumberOfOutPorts = 7,
  Widget           = 8,
};
Q_ENUM_NS(NodeRole)


enum NodeFlag
{
  NoFlags   = 0x0,
  Resizable = 0x1,
};

Q_DECLARE_FLAGS(NodeFlags, NodeFlag)
Q_FLAG_NS(NodeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(NodeFlags)


enum class PortRole
{
  Data             = 0,
  DataType         = 1,
  ConnectionPolicy = 2,
  CaptionVisible   = 3,
  Caption          = 4,
};
Q_ENUM_NS(PortRole)


enum class ConnectionPolicy
{
  One,
  Many,
};
Q_ENUM_NS(ConnectionPolicy)


enum class PortType
{
  In   = 0,
  Out  = 1,
  None = 2
};
Q_ENUM_NS(PortType)


using PortIndex = unsigned int;

static constexpr PortIndex InvalidPortIndex =
  std::numeric_limits<PortIndex>::max();

using NodeId = unsigned int;

static constexpr NodeId InvalidNodeId =
  std::numeric_limits<NodeId>::max();


/// A unique connection identificator that stores a tuple made of (out `NodeId`, out `PortIndex`, in `NodeId`, in * `PortIndex`)
using ConnectionId = std::tuple<NodeId, PortIndex,  // Port Out
                                NodeId, PortIndex>; // Port In

}
