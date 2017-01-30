#include "DataModelRegistry.hpp"

#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>

using QtNodes::DataModelRegistry;
using QtNodes::NodeDataModel;

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


std::unique_ptr<NodeDataModel>
DataModelRegistry::
getTypeConverter(const QString & sourceTypeID, const QString & destTypeID) const
{
  for (const auto& m : _registeredTypeConverters)
  {
    if (m.second->SourceType.id == sourceTypeID && m.second->DestinationType.id == destTypeID)
    {
      return m.second->Model->clone();
    }
  }
  
  return nullptr;
}