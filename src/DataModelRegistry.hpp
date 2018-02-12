#pragma once

#include <unordered_map>
#include <set>
#include <memory>
#include <functional>

#include <QtCore/QString>

#include "NodeDataModel.hpp"
#include "Export.hpp"
#include "QStringStdHash.hpp"

namespace QtNodes
{

/// Class uses map for storing models (name, model)
class NODE_EDITOR_PUBLIC DataModelRegistry
{

public:

  using RegistryItemPtr             = std::unique_ptr<NodeDataModel>;
  using RegistryItemCloner          = std::function<RegistryItemPtr()>;
  using RegisteredModelsMap         = std::unordered_map<QString, RegistryItemCloner>;
  using RegisteredModelsCategoryMap = std::unordered_map<QString, QString>;
  using CategoriesSet               = std::set<QString>;

  struct TypeConverterItem
  {
    RegistryItemCloner ModelCloner{};
    NodeDataType       SourceType{};
    NodeDataType       DestinationType{};
  };

  using ConvertingTypesPair = std::pair<QString, QString>; //Source type ID, Destination type ID in this order
  using TypeConverterItemPtr = std::unique_ptr<TypeConverterItem>;
  using RegisteredTypeConvertersMap = std::map<ConvertingTypesPair, TypeConverterItemPtr>;

  DataModelRegistry()  = default;
  ~DataModelRegistry() = default;

  DataModelRegistry(DataModelRegistry const &) = delete;
  DataModelRegistry(DataModelRegistry &&)      = default;

  DataModelRegistry&
  operator=(DataModelRegistry const &) = delete;
  DataModelRegistry&
  operator=(DataModelRegistry &&) = default;

public:

  template<typename ModelType, bool TypeConverter = false>
  void
  registerModel(RegistryItemCloner cloner = [](){ return std::make_unique<ModelType>(); },
                QString const &category = "Nodes")
  {
    static_assert(std::is_base_of<NodeDataModel, ModelType>::value,
                  "Must pass a subclass of NodeDataModel to registerModel");

    RegistryItemPtr prototypeInstance = cloner();
    QString const name = prototypeInstance->name();

    if (_registeredModelCreators.count(name) == 0)
    {
      _registeredModelCreators[name] = std::move(cloner);
      _categories.insert(category);
      _registeredModelsCategory[name] = category;
    }

    if (TypeConverter)
    {
      //Type converter node should have exactly one input and output ports, if thats not the case, we skip the registration.
      //If the input and output type is the same, we also skip registration, because thats not a typecast node.
      if (prototypeInstance->nPorts(PortType::In) != 1 || prototypeInstance->nPorts(PortType::Out) != 1 ||
        prototypeInstance->dataType(PortType::In, 0).id == prototypeInstance->dataType(PortType::Out, 0).id)
      {
        return;
      }

      TypeConverterItemPtr converter = std::make_unique<TypeConverterItem>();
      converter->ModelCloner = cloner;
      converter->SourceType = prototypeInstance->dataType(PortType::In, 0);
      converter->DestinationType = prototypeInstance->dataType(PortType::Out, 0);

      auto typeConverterKey = std::make_pair(converter->SourceType.id, converter->DestinationType.id);
	  _registeredTypeConverters[typeConverterKey] = std::move(converter);
    }
  }

  //Parameter order alias, so a category can be set without forcing to manually pass a model instance
  template<typename ModelType, bool TypeConverter = false>
  void
  registerModel(QString const &category, RegistryItemCloner cloner = [](){ return std::make_unique<ModelType>();})
  {
    registerModel<ModelType, TypeConverter>(std::move(cloner), category);
  }

  std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  RegisteredModelsMap const &
  registeredModels() const;
  
  RegisteredModelsCategoryMap const &
  registeredModelsCategoryAssociation() const;
  
  CategoriesSet const &
  categories() const;

  std::unique_ptr<NodeDataModel>
  getTypeConverter(QString const &sourceTypeID,
                   QString const &destTypeID) const;

private:

  RegisteredModelsCategoryMap _registeredModelsCategory{};
  CategoriesSet _categories{};
  RegisteredModelsMap _registeredModelCreators{};
  RegisteredTypeConvertersMap _registeredTypeConverters{};
};
}
