#pragma once

#include <unordered_map>

#include <QtCore/QString>

#include "NodeDataModel.hpp"
#include "Export.hpp"
#include "UniquePtr.hpp"
#include "QStringStdHash.hpp"

/// Base abstract class for Model Registry items
class RegistryItem
{
public:
  RegistryItem() {}

  virtual
  ~RegistryItem() {}

  virtual std::unique_ptr<NodeDataModel>
  create() const = 0;

  virtual QString
  name() const = 0;
};

//------------------------------------------------------------------------------

/// Encapsulate templated concrete Model type T
template<typename T>
class RegistryItemImpl : public RegistryItem
{

public:

  /// Gives derived classes the ability to create instances of T
  std::unique_ptr<NodeDataModel>
  create() const override
  { return std::make_unique<T>(); }

  QString
  name() const override
  { return T::name(); }
};

//------------------------------------------------------------------------------

/// Class uses static map for storing models (name, model)
class NODE_EDITOR_PUBLIC DataModelRegistry
{

public:

  using RegistryItemPtr     = std::unique_ptr<RegistryItem>;
  using RegisteredModelsMap =
          std::unordered_map<QString, RegistryItemPtr>;

public:

  template<typename ModelType>
  static void
  registerModel(QString menuName = QString())
  {
    QString const name = ModelType::name();

    if (_registeredModels.count(name) == 0)
    {
      auto uniqueModel =
        std::make_unique<RegistryItemImpl<ModelType> > ();

      _registeredModels[uniqueModel->name()] = std::move(uniqueModel);
    }
  }

  static std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

  static RegisteredModelsMap const &
  registeredModels();

private:

  static RegisteredModelsMap _registeredModels;
};
