#pragma once

#include <unordered_map>
#include <memory>

#include <QtCore/QString>

#include "NodeDataModel.hpp"
#include "Export.hpp"
#include "QStringStdHash.hpp"

/// Class uses map for storing models (name, model)
class NODE_EDITOR_PUBLIC DataModelRegistry
{

public:
  
  using RegistryItemPtr     = std::unique_ptr<NodeDataModel>;
  using RegisteredModelsMap = std::unordered_map<QString, RegistryItemPtr>;

  struct TypeConverterItem
  {
    RegistryItemPtr Model;
    NodeDataType    SourceType;
    NodeDataType    DestinationType;
  };

  using TypeConverterItemPtr = std::unique_ptr<TypeConverterItem>;
  using RegisteredTypeConvertersMap = std::unordered_map<QString, TypeConverterItemPtr>;

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
  registerModel(std::unique_ptr<ModelType> uniqueModel = std::make_unique<ModelType>())
  {
    static_assert(std::is_base_of<NodeDataModel, ModelType>::value,
                  "Must pass a subclass of NodeDataModel to registerModel");

    QString const name = uniqueModel->name();

    if (_registeredModels.count(name) == 0)
    {
      _registeredModels[name] = std::move(uniqueModel);
    }

    if (TypeConverter && _registeredTypeConverters.count(name) == 0)
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

      _registeredTypeConverters[name] = std::move(converter);
    }
  }

  std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  RegisteredModelsMap const &
  registeredModels() const;

  std::unique_ptr<NodeDataModel>
  getTypeConverter(const QString & sourceTypeID, 
                   const QString & destTypeID) const;

private:

  RegisteredModelsMap _registeredModels;
  RegisteredTypeConvertersMap _registeredTypeConverters;
};
