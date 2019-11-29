#include "NodeGroup.hpp"

#include <QtCore/QByteArray>
#include <QtWidgets/QFileDialog>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

NodeGroup::NodeGroup(const std::vector<QUuid>& nodeKeys,
                     const std::vector<QUuid>& connectionKeys,
                     std::unordered_map<QUuid, UniqueNode>& nodes,
                     std::unordered_map<QUuid, SharedConnection>& connections)
  : _uid(QUuid::createUuid()),
    _groupGraphicsObject(nullptr)
{
  stealNodes(nodeKeys, nodes);
  stealConnections(connectionKeys, connections);

  for (auto& node : _childNodes)
  {
    node.second->setNodeGroup(this);
  }
}

NodeGroup::~NodeGroup() = default;


QJsonObject NodeGroup::save() const
{
  QJsonObject groupJson;

  groupJson["id"] = _uid.toString();

  QJsonObject pos;
  pos["x"] = _groupGraphicsObject->pos().x();
  pos["y"] = _groupGraphicsObject->pos().y();
  groupJson["pos"] = pos;

  QJsonArray children;
  for (auto & child : _childNodes)
  {
    children.append(child.second->save());
  }
  groupJson["children"] = children;

  QJsonArray connections;
  for (auto & connection : _childConnections)
  {
    connections.append(connection.second->save());
  }
  groupJson["connections"] = connections;

  return groupJson;
}

void NodeGroup::restore(QJsonObject const& json) {}

void NodeGroup::saveGroupFile() const
{

}

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

std::unordered_map<QUuid, NodeGroup::UniqueNode> const &
NodeGroup::childNodes() const
{
  return _childNodes;
}

std::unordered_map<QUuid, NodeGroup::SharedConnection> const &
NodeGroup::childConnections() const
{
  return _childConnections;
}

bool NodeGroup::locked() const
{
  return _locked;
}

void NodeGroup::setGraphicsObject(
  std::unique_ptr<GroupGraphicsObject>&& graphics_object)
{
  _groupGraphicsObject = std::move(graphics_object);
  for (auto& node : _childNodes)
  {
    addNodeToGroup(node.second.get());
  }

  lock(true);
}

bool NodeGroup::empty() const
{
  return _childNodes.empty();
}

void
NodeGroup::
setSelected(bool selected)
{
  _groupGraphicsObject->setSelected(selected);
  for (auto& node : childNodes())
  {
    node.second->nodeGraphicsObject().setSelected(selected);
  }
}

void NodeGroup::lock(bool locked)
{
  for (auto& node : childNodes())
  {
    node.second->nodeGraphicsObject().lock(locked);
  }
  groupGraphicsObject().lock(locked);
  _locked = locked;
}

void NodeGroup::stealNodes(const std::vector<QUuid>& keys,
                           std::unordered_map<QUuid, NodeGroup::UniqueNode>& nodes)
{
  for (auto& key : keys)
  {
    auto nodeIt = nodes.find(key);
    auto handler = nodes.extract(nodeIt);
    _childNodes.insert(std::move(handler));
  }
}

void NodeGroup::stealConnections(const std::vector<QUuid>& keys,
                                 std::unordered_map<QUuid, NodeGroup::SharedConnection>& connections)
{
  for (auto& key : keys)
  {
    auto connectionIt = connections.find(key);
    auto handler = connections.extract(connectionIt);
    _childConnections.insert(std::move(handler));
  }
}

void NodeGroup::addNodeGraphicsObject(NodeGraphicsObject& ngo)
{
  _groupGraphicsObject->addObject(ngo);
}

void NodeGroup::addNodeToGroup(Node* node)
{
  /// TODO: check if any connection should be added to the group as well
  addNodeGraphicsObject(node->nodeGraphicsObject());
}

std::unordered_map<QUuid, NodeGroup::UniqueNode>::node_type
NodeGroup::
removeNodeFromGroup(QUuid nodeID)
{
  auto nodeIterator = _childNodes.find(nodeID);
  auto nodeHandler = _childNodes.extract(nodeIterator);
  groupGraphicsObject().positionLockedIcon();
  return nodeHandler;
}
