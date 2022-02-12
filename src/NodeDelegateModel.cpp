#include "NodeDelegateModel.hpp"

#include "StyleCollection.hpp"

namespace QtNodes
{

NodeDelegateModel::
NodeDelegateModel()
  : _nodeStyle(StyleCollection::nodeStyle())
{
  // Derived classes can initialize specific style here
}


QJsonObject
NodeDelegateModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


NodeStyle const &
NodeDelegateModel::
nodeStyle() const
{
  return _nodeStyle;
}


void
NodeDelegateModel::
setNodeStyle(NodeStyle const& style)
{
  _nodeStyle = style;
}


}
