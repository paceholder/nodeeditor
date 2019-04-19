#include "NodeDataModel.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeDataModel;
using QtNodes::NodeStyle;

NodeDataModel::
NodeDataModel()
  : _nodeStyle(StyleCollection::nodeStyle())
{
   // Derived classes can initialize specific style here
}

void NodeDataModel::setToolTipText(const QString& toolTipText)
{
   _toolTipText = toolTipText;
   Q_EMIT setToolTipTextSignal(toolTipText);
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
