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
                     QString const &category = "Nodes")
  {
    registerModelImpl<ModelType>(std::move(creator), category);
  }

  template<typename ModelType>
  void registerModel(QString const &category = "Nodes")
  {
    RegistryItemCreator creator = [](){ return std::make_unique<ModelType>(); };
    registerModelImpl<ModelType>(std::move(creator), category);
  }

  template<typename ModelType>
  void registerModel(QString const &category,
                     RegistryItemCreator creator)
  {
    registerModelImpl<ModelType>(std::move(creator), category);
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

private:

  // If the registered ModelType class has the static member method
  //
  //      static Qstring Name();
  //
  // use it. Otherwise use the non-static method:
  //
  //       virtual QString name() const;

  template <typename T, typename = void>
  struct HasStaticMethodName
      : std::false_type
  {};

  template <typename T>
  struct HasStaticMethodName<T,
          typename std::enable_if<std::is_same<decltype(T::Name()), QString>::value>::type>
      : std::true_type
  {};

  template<typename ModelType>
  typename std::enable_if< HasStaticMethodName<ModelType>::value>::type
  registerModelImpl(RegistryItemCreator creator, QString const &category )
  {
    const QString name = ModelType::Name();
    if (_registeredItemCreators.count(name) == 0)
    {
      _registeredItemCreators[name] = std::move(creator);
      _categories.insert(category);
      _registeredModelsCategory[name] = category;
    }
  }

  template<typename ModelType>
  typename std::enable_if< !HasStaticMethodName<ModelType>::value>::type
  registerModelImpl(RegistryItemCreator creator, QString const &category )
  {
    const QString name = creator()->name();
    if (_registeredItemCreators.count(name) == 0)
    {
      _registeredItemCreators[name] = std::move(creator);
      _categories.insert(category);
      _registeredModelsCategory[name] = category;
    }
  }

};



}
