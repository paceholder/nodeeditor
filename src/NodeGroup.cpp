#include "NodeGroup.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

NodeGroup::NodeGroup(const std::vector<Node*>& nodes,
                     const QString& name)
  : _uid(QUuid::createUuid()),
    _name(name),
    _childNodes(nodes),
    _groupGraphicsObject(nullptr)
{}

NodeGroup::~NodeGroup() = default;

QJsonObject NodeGroup::save() const {}

void NodeGroup::restore(QJsonObject const& json) {}

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
