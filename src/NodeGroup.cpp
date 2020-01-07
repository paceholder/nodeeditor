#include "NodeGroup.hpp"

#include <QJsonDocument>
#include <QJsonArray>

#include <utility>

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

int NodeGroup::_groupCount = 0;

NodeGroup::
NodeGroup(std::vector<Node*> nodes,
          QString name,
          QObject* parent)
  : QObject(parent)
  , _name(std::move(name))
  , _uid(QUuid::createUuid())
  , _childNodes(std::move(nodes))
  , _groupGraphicsObject(nullptr)
{
  _groupCount++;
}

QByteArray
NodeGroup::
saveToFile() const
{
  QJsonObject groupJson;

  groupJson["name"] = _name;
  groupJson["id"] = _uid.toString();

  QJsonArray nodesJson;
  for (auto const & node : _childNodes)
  {
    nodesJson.append(node->save());
  }
  groupJson["nodes"] = nodesJson;

  QJsonArray connectionsJson;
  auto groupConnections = _groupGraphicsObject->connections();
  for (auto const & connection : groupConnections)
  {
    connectionsJson.append(connection->save());
  }
  groupJson["connections"] = connectionsJson;

  QJsonDocument groupDocument(groupJson);

  return groupDocument.toJson();
}

QUuid
NodeGroup::
id() const
{
  return _uid;
}

GroupGraphicsObject&
NodeGroup::
groupGraphicsObject()
{
  return *_groupGraphicsObject;
}

GroupGraphicsObject const&
NodeGroup::
groupGraphicsObject() const
{
  return *_groupGraphicsObject;
}

std::vector<Node*>&
NodeGroup::
childNodes()
{
  return _childNodes;
}

std::vector<QUuid>
NodeGroup::
nodeIDs() const
{
  std::vector<QUuid> ret{};
  ret.reserve(_childNodes.size());

  for (auto const & node : _childNodes)
  {
    ret.push_back(node->id());
  }

  return ret;
}

QString const&
NodeGroup::name() const
{
  return _name;
}

void
NodeGroup::
setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object)
{
  _groupGraphicsObject = std::move(graphics_object);
  _groupGraphicsObject->lock(true);
}

bool
NodeGroup::
empty() const
{
  return _childNodes.empty();
}

int
NodeGroup::
groupCount()
{
  return _groupCount;
}

void
NodeGroup::
addNode(Node* node)
{
  _childNodes.push_back(node);
}

void
NodeGroup::
removeNode(Node* node)
{
  auto nodeIt = std::find(_childNodes.begin(), _childNodes.end(), node);
  if (nodeIt != _childNodes.end())
  {
    (*nodeIt)->unsetNodeGroup();
    _childNodes.erase(nodeIt);
    groupGraphicsObject().positionLockedIcon();
  }
}
