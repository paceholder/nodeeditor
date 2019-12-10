#include "NodeGroup.hpp"

#include <QJsonDocument>
#include <QJsonArray>

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

int NodeGroup::_groupCount = 0;

NodeGroup::NodeGroup(const std::vector<Node*>& nodes,
                     const QString& name)
  : _uid(QUuid::createUuid()),
    _name(name),
    _childNodes(nodes),
    _groupGraphicsObject(nullptr)
{
  _groupCount++;
}

NodeGroup::~NodeGroup() = default;

QByteArray NodeGroup::saveToFile() const
{
  QJsonObject groupJson;

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

void NodeGroup::restoreFromFile(QJsonObject const& json) {}

QUuid NodeGroup::id() const
{
  return _uid;
}

GroupGraphicsObject const& NodeGroup::groupGraphicsObject() const
{
  return *_groupGraphicsObject.get();
}

GroupGraphicsObject& NodeGroup::groupGraphicsObject()
{
  return *_groupGraphicsObject.get();
}

std::vector<Node*>& NodeGroup::childNodes()
{
  return _childNodes;
}

std::vector<QUuid> NodeGroup::nodeIDs() const
{
  std::vector<QUuid> ret{};

  for (auto const & node : _childNodes)
  {
    ret.push_back(node->id());
  }

  return ret;
}

const QString& NodeGroup::name() const
{
  return _name;
}

void NodeGroup::setGraphicsObject(
  std::unique_ptr<GroupGraphicsObject>&& graphics_object)
{
  _groupGraphicsObject = std::move(graphics_object);
  _groupGraphicsObject->lock(true);
}

bool NodeGroup::empty() const
{
  return _childNodes.empty();
}

int NodeGroup::groupCount()
{
  return _groupCount;
}

void NodeGroup::addNode(Node* node)
{
  _childNodes.push_back(node);
}

void NodeGroup::removeNode(Node* node)
{
  auto nodeIt = std::find(_childNodes.begin(), _childNodes.end(), node);
  if (nodeIt != _childNodes.end())
  {
    _childNodes.erase(nodeIt);
    groupGraphicsObject().positionLockedIcon();
  }
}
