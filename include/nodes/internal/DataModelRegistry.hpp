#pragma once

#include <set>
#include <memory>
#include <functional>
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
  using RegistryItemCreator = std::function<RegistryItemPtr()>;
  using RegisteredModelCreatorsMap = std::unordered_map<QString, RegistryItemCreator>;
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

  void registerModel(RegistryItemCreator creator,
                     QString const &category = "Nodes");

  template<typename ModelType>
  void registerModel(QString const &category,
                     RegistryItemCreator creator)
  {
    registerModel<ModelType>(std::move(creator), category);
  }

  template<typename ModelType>
  void registerModel(QString const &category = "Nodes")
  {
    RegistryItemCreator creator = [](){ return detail::make_unique<ModelType>(); };
    registerModel<ModelType>(std::move(creator), category);
  }

  void registerTypeConverter(TypeConverterId const & id,
                             TypeConverter typeConverter)
  {
    _registeredTypeConverters[id] = std::move(typeConverter);
  }

  std::unique_ptr<NodeDataModel>create(QString const &modelName);

  RegisteredModelCreatorsMap const &registeredModelCreators() const;

  RegisteredModelsCategoryMap const &registeredModelsCategoryAssociation() const;

  CategoriesSet const &categories() const;

  TypeConverter getTypeConverter(NodeDataType const & d1,
                                 NodeDataType const & d2) const;

private:

  RegisteredModelsCategoryMap _registeredModelsCategory;

  CategoriesSet _categories;

  RegisteredModelCreatorsMap _registeredItemCreators;

  RegisteredTypeConvertersMap _registeredTypeConverters;
};



template<typename ModelType> inline void
DataModelRegistry::
    registerModel(RegistryItemCreator creator, QString const &category)
{
  static_assert(std::is_base_of<NodeDataModel, ModelType>::value,
                "Must pass a subclass of NodeDataModel to registerModel");

  RegistryItemPtr prototypeInstance = creator();
  QString const name = prototypeInstance->name();

  if (_registeredItemCreators.count(name) == 0)
  {
    _registeredItemCreators[name] = std::move(creator);
    _categories.insert(category);
    _registeredModelsCategory[name] = category;
  }
}

}
