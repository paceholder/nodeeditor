#ifndef DATA_MODEL_REGISTRY_HPP
#define DATA_MODEL_REGISTRY_HPP

#include <unordered_map>

#include <QtCore/QString>

#include "NodeDataModel.hpp"

class RegistryItem
{
public:
  RegistryItem() {}

  virtual ~RegistryItem() {}

  // a way for derived classes to identify themselves
  virtual QString getName() const = 0;

  virtual NodeDataModel* create() = 0;
};

//------------------------------------------------------------------------------

template<typename T>
class RegistryItemImpl : public RegistryItem
{
public:
  // get the identifier of the derived class
  QString getName() const override
  {
    return T::_name;
  }

  // give derived classes the ability to create themselves
  NodeDataModel *create() override { return new T(); }
};

//------------------------------------------------------------------------------

class DataModelRegistry
{
public:
  template<typename ModelType>
  static void registerModel(std::unique_ptr<RegistryItem> modelEntry)
  {
    if (_registeredModels.count(modelEntry->getName() == 0))
    {
      _registeredModels[modelEntry->getName()] = modelEntry;
    }
  }

  static std::unique_ptr<NodeDataModel>
  create(QString name)
  {
    auto it = _registeredModels.find(name);

    if (it != _registeredModels.end())
    {
      return std::unique_ptr<NodeDataModel>(it->second->create());
    }

    return std::unique_ptr<NodeDataModel>();
  }

private:

  static
  std::unordered_map<QString, std::unique_ptr<RegistryItem > > _registeredModels;
};


#endif // DATA_MODEL_REGISTRY_HPP
