#include "DataModelRegistry.hpp"

#include <algorithm>

#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>

using QtNodes::DataModelRegistry;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::TypeConverter;

std::unique_ptr<NodeDataModel>
DataModelRegistry::
create(QString const &modelName)
{
  auto it = _registeredItemCreators.find(modelName);

  if (it != _registeredItemCreators.end())
  {
    return it->second();
  }

  return nullptr;
}


DataModelRegistry::RegisteredModelCreatorsMap const &
DataModelRegistry::
registeredModelCreators() const
{
  return _registeredItemCreators;
}

DataModelRegistry::RegisteredModelsOrder const &
DataModelRegistry::
registeredModelsOrder() const
{
  return _registeredModelsOrder;
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


DataModelRegistry::CategoriesOrder const &
DataModelRegistry::
categoriesOrder() const
{
  return _categoriesOrder;
}

void
DataModelRegistry::
sortCategories()
{
  std::sort(_categoriesOrder.begin(), _categoriesOrder.end());
}


TypeConverter
DataModelRegistry::
getTypeConverter(NodeDataType const & d1,
                 NodeDataType const & d2) const
{
  TypeConverterId converterId = std::make_pair(d1, d2);

  auto it = _registeredTypeConverters.find(converterId);

  if (it != _registeredTypeConverters.end())
  {
    return it->second;
  }

  return TypeConverter{};
}
