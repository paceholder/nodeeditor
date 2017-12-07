#include "DataModelRegistry.hpp"

#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>

using QtNodes::DataModelRegistry;
using QtNodes::NodeDataModel;

std::unique_ptr<NodeDataModel>
DataModelRegistry::
create(QString const &modelName)
{
  auto it = _registeredModelCreators.find(modelName);

  if (it != _registeredModelCreators.end())
  {
    return it->second();
  }

  return nullptr;
}


DataModelRegistry::RegisteredModelsMap const &
DataModelRegistry::
registeredModels() const
{
  return _registeredModelCreators;
}


DataModelRegistry::RegisteredModelsCategoryMap const &
DataModelRegistry::
registeredModelsCategoryAssociation() const
{
  return _registeredModelsCategory;
}


DataModelRegistry::CategoriesSet const &
DataModelRegistry::
categories() const
{
  return _categories;
}


std::unique_ptr<NodeDataModel>
DataModelRegistry::
getTypeConverter(QString const &sourceTypeID, QString const &destTypeID) const
{
  auto typeConverterKey = std::make_pair(sourceTypeID, destTypeID);
  auto converter = _registeredTypeConverters.find(typeConverterKey);

  if (converter != _registeredTypeConverters.end())
  {
    return converter->second->ModelCloner();
  }
  return nullptr;
}
