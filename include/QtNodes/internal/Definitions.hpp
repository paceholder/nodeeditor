#pragma once

#include "Export.hpp"

#include <QtCore/QMetaObject>

#include <limits>

/**
 * @file
 * Important definitions used throughout the library.
 */

namespace QtNodes {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
NODE_EDITOR_PUBLIC Q_NAMESPACE
#else
Q_NAMESPACE_EXPORT(NODE_EDITOR_PUBLIC)
#endif

    /**
 * Constants used for fetching QVariant data from GraphModel.
 */
    enum class NodeRole {
        Type = 0,           ///< Type of the current node, usually a string.
        Position = 1,       ///< `QPointF` positon of the node on the scene.
        Size = 2,           ///< `QSize` for resizable nodes.
        CaptionVisible = 3, ///< `bool` for caption visibility.
        Caption = 4,        ///< `QString` for node caption.
        Style = 5,          ///< Custom NodeStyle as QJsonDocument
        InternalData = 6,   ///< Node-stecific user data as QJsonObject
        InPortCount = 7,    ///< `unsigned int`
        OutPortCount = 9,   ///< `unsigned int`
        Widget = 10,        ///< Optional `QWidget*` or `nullptr`
    };
Q_ENUM_NS(NodeRole)

/**
 * Specific flags regulating node features and appeaarence.
 */
enum NodeFlag {
    NoFlags = 0x0,   ///< Default NodeFlag
    Resizable = 0x1, ///< Lets the node be resizable
    Locked = 0x2
};

Q_DECLARE_FLAGS(NodeFlags, NodeFlag)
Q_FLAG_NS(NodeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(NodeFlags)

/**
 * Constants for fetching port-related information from the GraphModel.
 */
enum class PortRole {
    Data = 0,                 ///< `std::shared_ptr<NodeData>`.
    DataType = 1,             ///< `QString` describing the port data type.
    ConnectionPolicyRole = 2, ///< `enum` ConnectionPolicyRole
    CaptionVisible = 3,       ///< `bool` for caption visibility.
    Caption = 4,              ///< `QString` for port caption.
};
Q_ENUM_NS(PortRole)

/**
 * Defines how many connections are possible to attach to ports. The
 * values are fetched using PortRole::ConnectionPolicy.
 */
enum class ConnectionPolicy {
    One,  ///< Just one connection for each port.
    Many, ///< Any number of connections possible for the port.
};
Q_ENUM_NS(ConnectionPolicy)

/**
 * Used for distinguishing input and output node ports.
 */
enum class PortType {
    In = 0,  ///< Input node port (from the left).
    Out = 1, ///< Output node port (from the right).
    None = 2
};
Q_ENUM_NS(PortType)

using PortCount = unsigned int;

/// ports are consecutively numbered starting from zero.
using PortIndex = unsigned int;

static constexpr PortIndex InvalidPortIndex = std::numeric_limits<PortIndex>::max();

/// Unique Id associated with each node in the GraphModel.
using NodeId = unsigned int;

static constexpr NodeId InvalidNodeId = std::numeric_limits<NodeId>::max();

/**
 * A unique connection identificator that stores
 * out `NodeId`, out `PortIndex`, in `NodeId`, in `PortIndex`
 */
struct ConnectionId
{
    NodeId outNodeId;
    PortIndex outPortIndex;
    NodeId inNodeId;
    PortIndex inPortIndex;
};

inline bool operator==(ConnectionId const &a, ConnectionId const &b)
{
    return a.outNodeId == b.outNodeId && a.outPortIndex == b.outPortIndex
           && a.inNodeId == b.inNodeId && a.inPortIndex == b.inPortIndex;
}

inline bool operator!=(ConnectionId const &a, ConnectionId const &b)
{
    return !(a == b);
}

inline void invertConnection(ConnectionId &id)
{
    std::swap(id.outNodeId, id.inNodeId);
    std::swap(id.outPortIndex, id.inPortIndex);
}

} // namespace QtNodes
