#include "NodeGroup.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeGroup;

NodeGroup::NodeGroup(const std::vector<Node*>& nodes)
  : _uid(QUuid::createUuid()),
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
  groupGraphicsObject().lock(locked);
  _locked = locked;
}

void NodeGroup::addNodeGraphicsObject(NodeGraphicsObject& ngo)
{
  _groupGraphicsObject->addObject(ngo);
}

void NodeGroup::addNode(Node* node)
{
  addNodeGraphicsObject(node->nodeGraphicsObject());
}

void NodeGroup::removeNodeFromGroup(QUuid nodeID)
{
  // since we're using vectors, this operation is inefficient.
  // might be good to change it to an std::map<QUuid, node>.
  for (auto nodeIt = childNodes().begin(); nodeIt != childNodes().end(); ++nodeIt)
  {
    if ((*nodeIt)->id() == nodeID)
    {
      childNodes().erase(nodeIt);
      groupGraphicsObject().positionLockedIcon();
      return;
    }
  }
}
