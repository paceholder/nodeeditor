#include "NodeDataModel.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeDataModel;
using QtNodes::NodeStyle;

NodeDataModel::
NodeDataModel()
  : m_wembed(false), _nodeStyle(StyleCollection::nodeStyle())

{
  // Derived classes can initialize specific style here
}


QJsonObject
NodeDataModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


NodeStyle const&
NodeDataModel::
nodeStyle() const
{
  return _nodeStyle;
}


void
NodeDataModel::
setNodeStyle(NodeStyle const& style)
{
  _nodeStyle = style;
}

bool NodeDataModel::wembed() const
{
    return m_wembed;
}

void NodeDataModel::setWembed(bool wembed)
{
    m_wembed = wembed;
}
