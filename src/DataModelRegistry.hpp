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

  virtual std::unique_ptr<NodeDataModel> create() const = 0;
};

//------------------------------------------------------------------------------

template<typename T, typename ... Args>
std::unique_ptr<T> make_unique( Args&& ... args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>(args) ... ) );
}


//------------------------------------------------------------------------------

template<typename T>
class RegistryItemImpl : public RegistryItem
{

public:

  // give derived classes the ability to create themselves
  std::unique_ptr<NodeDataModel> create() const override
  { return make_unique<T>(); }
};

//------------------------------------------------------------------------------

namespace std
{
template<>
struct hash<QString>
{
  inline
  size_t operator()(QString const& s) const
  {
    return qHash(s);
  }
};
}

//------------------------------------------------------------------------------

class DataModelRegistry
{

public:

  using RegistryItemPtr = std::unique_ptr<RegistryItem>;
  using RegisteredModelsMap =
          std::unordered_map<QString, RegistryItemPtr>;

public:

  template<typename ModelType>
  static void registerModel(QString const &modelName)
  {
    if (_registeredModels.count(modelName) == 0)
    {
      //_registeredModels.insert(
        //std::make_pair(modelName,
                       //make_unique<RegistryItemImpl<ModelType>>()));
    }
  }

  static std::unique_ptr<NodeDataModel>
  create(QString const &modelName);

private:


  static RegisteredModelsMap _registeredModels;
};

#endif // DATA_MODEL_REGISTRY_HPP
