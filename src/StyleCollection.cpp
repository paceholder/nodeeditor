#include "StyleCollection.hpp"

using QtNodes::StyleCollection;
using QtNodes::NodeStyle;
using QtNodes::ConnectionStyle;
using QtNodes::FlowViewStyle;

NodeStyle const&
StyleCollection::
nodeStyle()
{
  return instance()._nodeStyle;
}


ConnectionStyle const&
StyleCollection::
connectionStyle()
{
  return instance()._connectionStyle;
}


FlowViewStyle const&
StyleCollection::
flowViewStyle()
{
  return instance()._flowViewStyle;
}


void
StyleCollection::
setNodeStyle(const QtNodes::NodeStyle& nodeStyle)
{
  instance()._nodeStyle = nodeStyle;
}


void
StyleCollection::
setConnectionStyle(const QtNodes::ConnectionStyle& connectionStyle)
{
  instance()._connectionStyle = connectionStyle;
}


void
StyleCollection::
setFlowViewStyle(const QtNodes::FlowViewStyle& flowViewStyle)
{
  instance()._flowViewStyle = flowViewStyle;
}



StyleCollection&
StyleCollection::
instance()
{
  static StyleCollection collection;

  return collection;
}
