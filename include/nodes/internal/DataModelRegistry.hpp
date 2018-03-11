#pragma once

#include <unordered_map>
#include <set>
#include <memory>

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
  using RegisteredModelsMap         = std::unordered_map<QString, RegistryItemPtr>;
  using RegisteredModelsCategoryMap = std::unordered_map<QString, QString>;
  using CategoriesSet               = std::set<QString>;

  struct TypeConverterItem
  {
    RegistryItemPtr Model{};
    NodeDataType    SourceType{};
    NodeDataType    DestinationType{};
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
  registerModel(std::unique_ptr<ModelType> uniqueModel = std::make_unique<ModelType>(), QString const &category = "Nodes")
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

    if (TypeConverter)
    {
      std::unique_ptr<NodeDataModel>& registeredModelRef = _registeredModels[name];

      //Type converter node should have exactly one input and output ports, if thats not the case, we skip the registration.
      //If the input and output type is the same, we also skip registration, because thats not a typecast node.
      if (registeredModelRef->nPorts(PortType::In) != 1 || registeredModelRef->nPorts(PortType::Out) != 1 ||
        registeredModelRef->dataType(PortType::In, 0).id == registeredModelRef->dataType(PortType::Out, 0).id)
      {
        return;
      }

      TypeConverterItemPtr converter = std::make_unique<TypeConverterItem>();
      converter->Model = registeredModelRef->clone();
      converter->SourceType = converter->Model->dataType(PortType::In, 0);
      converter->DestinationType = converter->Model->dataType(PortType::Out, 0);

      auto typeConverterKey = std::make_pair(converter->SourceType.id, converter->DestinationType.id);
	  _registeredTypeConverters[typeConverterKey] = std::move(converter);
    }
  }

  //Parameter order alias, so a category can be set without forcing to manually pass a model instance
  template<typename ModelType, bool TypeConverter = false>
  void
  registerModel(QString const &category, std::unique_ptr<ModelType> uniqueModel = std::make_unique<ModelType>())
  {
    registerModel<ModelType, TypeConverter>(std::move(uniqueModel), category);
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
  RegisteredModelsMap _registeredModels{};
  RegisteredTypeConvertersMap _registeredTypeConverters{};
};
}
