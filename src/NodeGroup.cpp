#include "NodeGroup.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

NodeGroup::NodeGroup(std::vector<Node*>&& nodes)
  : _uid(QUuid::createUuid()),
    _childNodes(std::move(nodes)),
    _groupGraphicsObject(nullptr)
{

  for (auto& node : _childNodes)
  {
    node->setNodeGroup(this);
  }
}

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

std::vector<Node*> const NodeGroup::childNodes() const
{
  return _childNodes;
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
    addNode(node);
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
    node->nodeGraphicsObject().setSelected(selected);
  }
}

void NodeGroup::lock(bool locked)
{
  for (auto& node : childNodes())
  {
    node->nodeGraphicsObject().lock(locked);
  }
  _locked = locked;
  if (locked)
  {
    groupGraphicsObject().setColor(groupGraphicsObject().kLockedFillColor);
  }
  else
  {
    groupGraphicsObject().setColor(groupGraphicsObject().kUnlockedFillColor);
  }
}

void NodeGroup::addNodeGraphicsObject(NodeGraphicsObject& ngo)
{
  _groupGraphicsObject->addObject(ngo);
}

void NodeGroup::addNode(Node* node)
{
  addNodeGraphicsObject(node->nodeGraphicsObject());
}

void NodeGroup::removeNode(Node* node)
{
  // remove node from list and update the bounding rectangle
}
