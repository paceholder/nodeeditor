#pragma once

#include <unordered_map>

#include <QtCore/QString>

#include "NodeDataModel.hpp"
#include "Export.hpp"
#include "UniquePtr.hpp"
#include "QStringStdHash.hpp"

/// Class uses static map for storing models (name, model)
class NODE_EDITOR_PUBLIC DataModelRegistry
{

public:

  using RegistryItemPtr     = std::unique_ptr<NodeDataModel>;
  using RegisteredModelsMap =
          std::unordered_map<QString, RegistryItemPtr>;

public:

  template<typename ModelType>
  static void
  registerModel(std::unique_ptr<ModelType> type)
  {
    static_assert(std::is_base_of<NodeDataModel, ModelType>::value, "Must pass a subclass of NodeDataModel to registerModel");

    QString const name = type->name();

    if (_registeredModels.count(name) == 0)
    {

      _registeredModels[name] = std::move(type);
    }
  }

  static std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  static RegisteredModelsMap const &
  registeredModels();

private:

  static RegisteredModelsMap _registeredModels;
};
