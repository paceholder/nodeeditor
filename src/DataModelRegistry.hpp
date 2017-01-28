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

  // TODO rewrite the TypeConverter hack to use a constexpr if once C++17 is supported enough. 
  // (This probably could be done with enable_if, but that would be more redundant, and ugly.)
  template<typename ModelType, bool TypeConverter = false>
  void
  registerModel(std::unique_ptr<ModelType> uniqueModel = std::make_unique<ModelType>(), bool isTypeConverter = TypeConverter)
  {
    static_assert(std::is_base_of<NodeDataModel, ModelType>::value,
                  "Must pass a subclass of NodeDataModel to registerModel");

    QString const name = uniqueModel->name();

    if (_registeredModels.count(name) == 0)
    {
      _registeredModels[name] = std::move(uniqueModel);
    }

    if (isTypeConverter && _registeredTypeConverters.count(name) == 0)
    {
      TypeConverterItemPtr converter = std::make_unique<TypeConverterItem>();
      converter->Model = _registeredModels[name]->clone();
      converter->SourceType = converter->Model->dataType(PortType::In, 0);
      converter->DestinationType = converter->Model->dataType(PortType::Out, 0);
      _registeredTypeConverters[name] = std::move(converter);
    }
  }

  std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  RegisteredModelsMap const &
  registeredModels() const;

  bool 
  getTypeConverter(const QString & sourceTypeID, 
                   const QString & destTypeID,
                   std::unique_ptr<NodeDataModel> & converterModel) const;

private:

  RegisteredModelsMap _registeredModels;
  RegisteredTypeConvertersMap _registeredTypeConverters;
};
