#pragma once

#include "NodeStyle.hpp"
#include "ConnectionStyle.hpp"
#include "FlowViewStyle.hpp"

namespace QtNodes
{

class StyleCollection
{
public:

  static
  NodeStyle const&
  nodeStyle();

  static
  ConnectionStyle const&
  connectionStyle();

  static
  FlowViewStyle const&
  flowViewStyle();

public:

  static
  void
  setNodeStyle(NodeStyle);

  static
  void
  setConnectionStyle(ConnectionStyle);

  static
  void
  setFlowViewStyle(FlowViewStyle);

private:

  StyleCollection();

private:
  StyleCollection(StyleCollection const&);
  StyleCollection&
  operator=(StyleCollection const&);

private:
  static
  StyleCollection&
  instance();

private:

  NodeStyle _nodeStyle;

  ConnectionStyle _connectionStyle;

  FlowViewStyle _flowViewStyle;
};
}
