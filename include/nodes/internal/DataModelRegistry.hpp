#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include <QtCore/QString>

#include "NodeDataModel.hpp"
#include "TypeConverter.hpp"
#include "Export.hpp"
#include "QStringStdHash.hpp"
#include "memory.hpp"

namespace QtNodes
{

inline
bool
operator<(QtNodes::NodeDataType const & d1,
          QtNodes::NodeDataType const & d2)
{
  return d1.id < d2.id;
}


/// Class uses map for storing models (name, model)
class NODE_EDITOR_PUBLIC DataModelRegistry
{

public:

  using RegistryItemPtr     = std::unique_ptr<NodeDataModel>;
  using RegisteredModelsMap = std::unordered_map<QString, RegistryItemPtr>;
  using RegisteredModelsCategoryMap = std::unordered_map<QString, QString>;
  using CategoriesSet = std::set<QString>;

  using RegisteredTypeConvertersMap = std::map<TypeConverterId, TypeConverter>;

  DataModelRegistry()  = default;
  ~DataModelRegistry() = default;

  DataModelRegistry(DataModelRegistry const &) = delete;
  DataModelRegistry(DataModelRegistry &&)      = default;

  DataModelRegistry&operator=(DataModelRegistry const &) = delete;
  DataModelRegistry&operator=(DataModelRegistry &&)      = default;

public:

  template<typename ModelType>
  void registerModel(std::unique_ptr<ModelType> uniqueModel =
                       detail::make_unique<ModelType>(),
                     QString const &category = "Nodes")
  {
    static_assert(std::is_base_of<NodeDataModel, ModelType>::value,
                  "Must pass a subclass of NodeDataModel to registerModel");

    QString const name = uniqueModel->name();

    if (_registeredModels.count(name) == 0)
    {
      _registeredModels[name] = std::move(uniqueModel);
      _categories.insert(category);
      _registeredModelsCategory[name] = category;
    }
  }

  //Parameter order alias, so a category can be set without forcing to manually pass a model instance
  template<typename ModelType>
  void registerModel(QString const &category, std::unique_ptr<ModelType> uniqueModel = detail::make_unique<ModelType>())
  {
    registerModel<ModelType>(std::move(uniqueModel), category);
  }

  void registerTypeConverter(TypeConverterId const & id,
                             TypeConverter typeConverter)
  {
    _registeredTypeConverters[id] = std::move(typeConverter);
  }

  std::unique_ptr<NodeDataModel>create(QString const &modelName);

  RegisteredModelsMap const &registeredModels() const;

  RegisteredModelsCategoryMap const &registeredModelsCategoryAssociation() const;

  CategoriesSet const &categories() const;

  TypeConverter getTypeConverter(NodeDataType const & d1,
                                 NodeDataType const & d2) const;

private:

  RegisteredModelsCategoryMap _registeredModelsCategory;

  CategoriesSet _categories;

  RegisteredModelsMap _registeredModels;

  RegisteredTypeConvertersMap _registeredTypeConverters;
};
}
