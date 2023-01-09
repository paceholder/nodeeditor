#pragma once

#include "Export.hpp"

#include "ConnectionStyle.hpp"
#include "GraphicsViewStyle.hpp"
#include "NodeStyle.hpp"

namespace QtNodes {

class NODE_EDITOR_PUBLIC StyleCollection
{
public:
    static NodeStyle const &nodeStyle();

    static ConnectionStyle const &connectionStyle();

    static GraphicsViewStyle const &flowViewStyle();

public:
    static void setNodeStyle(NodeStyle);

    static void setConnectionStyle(ConnectionStyle);

    static void setGraphicsViewStyle(GraphicsViewStyle);

private:
    StyleCollection() = default;

    StyleCollection(StyleCollection const &) = delete;

    StyleCollection &operator=(StyleCollection const &) = delete;

    static StyleCollection &instance();

private:
    NodeStyle _nodeStyle;

    ConnectionStyle _connectionStyle;

    GraphicsViewStyle _flowViewStyle;
};
} // namespace QtNodes
