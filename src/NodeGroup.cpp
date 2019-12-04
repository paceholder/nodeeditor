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
{}

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
