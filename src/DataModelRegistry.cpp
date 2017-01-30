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
registeredModels() const
{
  return _registeredModels;
}


std::unique_ptr<NodeDataModel>
DataModelRegistry::
getTypeConverter(const QString & sourceTypeID, const QString & destTypeID) const
{
  auto typeConverterKey = std::make_pair(sourceTypeID, destTypeID);
  auto converter = _registeredTypeConverters.find(typeConverterKey);

  if (converter != _registeredTypeConverters.end())
  {
    return converter->second->Model->clone();
  }
  return nullptr;
}