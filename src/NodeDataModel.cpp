#include "NodeDataModel.hpp"

#include <utility>

using QtNodes::NodeDataModel;
using QtNodes::NodeStyle;

NodeDataModel::
NodeDataModel(std::shared_ptr<NodeStyle const> style)
  : _nodeStyle(std::move(style))
{
  // Derived classes can initialize specific style here
}


QJsonObject
NodeDataModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


NodeStyle const&
NodeDataModel::
nodeStyle() const
{
  return *_nodeStyle;
}


void
NodeDataModel::
setNodeStyle(std::shared_ptr<NodeStyle const> style)
{
  _nodeStyle = std::move(style);
}
