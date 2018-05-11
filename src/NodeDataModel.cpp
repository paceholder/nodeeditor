#include "NodeDataModel.hpp"

#include <utility>

using QtNodes::NodeDataModel;

NodeDataModel::
NodeDataModel()
= default;


QJsonObject
NodeDataModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}
