#include "NodeDataModel.hpp"

#include <utility>

using QtNodes::NodeDataModel;

NodeDataModel::
NodeDataModel()
{
}


QJsonObject
NodeDataModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}
