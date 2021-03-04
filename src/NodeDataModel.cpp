#include "NodeDataModel.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeDataModel;
using QtNodes::NodeStyle;
using QtNodes::NodeData;
using QtNodes::PortIndex;

NodeDataModel::
NodeDataModel()
  : _nodeStyle(StyleCollection::nodeStyle())
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

void NodeDataModel::
setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
    if (portInConnectionPolicy(port) == QtNodes::NodeDataModel::ConnectionPolicy::One)
    {
        if (nodeData.empty())
            setInData(nullptr, port);
        else
            setInData(nodeData[0], port);
    }
    else
    {
        Q_ASSERT(false);
    }
}
