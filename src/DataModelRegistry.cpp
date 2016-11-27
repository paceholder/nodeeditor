#include "DataModelRegistry.hpp"

#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>

std::unique_ptr<NodeDataModel>
DataModelRegistry::
create(QString const &modelName)
{
  auto it = _registeredModels.find(modelName);

  if (it != _registeredModels.end())
  {
    return it->second->clone();
  }

  return nullptr;
}


DataModelRegistry::RegisteredModelsMap const &
DataModelRegistry::
registeredModels()
{
  return _registeredModels;
}
