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
  using RegisteredModelsMap =
          std::unordered_map<QString, RegistryItemPtr>;

  DataModelRegistry()  = default;
  ~DataModelRegistry() = default;

  DataModelRegistry(DataModelRegistry const &) = delete;
  DataModelRegistry(DataModelRegistry &&)      = default;

  DataModelRegistry&
  operator=(DataModelRegistry const &) = delete;
  DataModelRegistry&
  operator=(DataModelRegistry &&) = default;

public:

  template<typename ModelType>
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
  }

  std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  RegisteredModelsMap const &
  registeredModels();

private:

  RegisteredModelsMap _registeredModels;
};
