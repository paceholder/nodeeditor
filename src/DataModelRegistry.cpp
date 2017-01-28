#include "DataModelRegistry.hpp"

#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>

std::unique_ptr<NodeDataModel>
DataModelRegistry::
create(QString const &modelName)
{
  auto it = _registeredModels.find(modelName);

  if (it != _registeredModels.end())
  {
    return it->second->clone();
  }

  return nullptr;
}


DataModelRegistry::RegisteredModelsMap const &
DataModelRegistry::
registeredModels() const
{
  return _registeredModels;
}


bool
DataModelRegistry::
getTypeConverter(const QString & sourceTypeID, const QString & destTypeID, std::unique_ptr<NodeDataModel> & converterModel) const
{
  auto converter = std::find_if(_registeredTypeConverters.begin(), _registeredTypeConverters.end(),
    [&](const RegisteredTypeConvertersMap::value_type & m)
    {
      return m.second->SourceType.id == sourceTypeID && m.second->DestinationType.id == destTypeID;
    });

  if (converter == _registeredTypeConverters.end())
  {
    return false;
  }
  
  converterModel = converter->second->Model->clone();
  
  return true;
}