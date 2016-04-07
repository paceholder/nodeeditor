#include "DataModelRegistry.hpp"

DataModelRegistry::
RegisteredModelsMap DataModelRegistry::_registeredModels;

std::unique_ptr<NodeDataModel>
DataModelRegistry::
create(QString const &modelName)
{
  auto it = _registeredModels.find(modelName);

  if (it != _registeredModels.end())
  {
    return it->second->create();
  }

  return std::unique_ptr<NodeDataModel>();
}


DataModelRegistry::RegisteredModelsMap const &
DataModelRegistry::
registeredModels()
{
  return _registeredModels;
}
