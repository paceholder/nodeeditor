#include "NodeGroup.hpp"

using QtNodes::Node;
using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

NodeGroup::
NodeGroup(std::vector<Node*>&& nodes)
  : _uid(QUuid::createUuid())
  , _childNodes(std::move(nodes))
  , _groupGraphicsObject(nullptr)
{}

NodeGroup::
~NodeGroup() = default;

QJsonObject
NodeGroup::
save() const
{}

void
NodeGroup::
restore(QJsonObject const & json)
{}

QUuid
NodeGroup::
id() const
{
  return _uid;
}

GroupGraphicsObject const &
NodeGroup::
groupGraphicsObject() const
{
  return *_groupGraphicsObject.get();
}

GroupGraphicsObject &
NodeGroup::
groupGraphicsObject()
{
  return *_groupGraphicsObject.get();
}

std::vector<Node*> const
NodeGroup::
childNodes() const
{
  return _childNodes;
}

void
NodeGroup::
setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object)
{
  _groupGraphicsObject = std::move(graphics_object);
//  for(auto& node : _childNodes)
//  {
//    auto initialNodePos = node->nodeGraphicsObject().pos();
//    node->nodeGraphicsObject().setParentItem(_groupGraphicsObject.get());
//    node->nodeGraphicsObject().setPos(-_groupGraphicsObject->pos() + initialNodePos);
//  }
}

bool
NodeGroup::
empty() const
{
  return _childNodes.empty();
}

void
NodeGroup::
addNode(const QUuid& node_id)
{
  // add node to list and update the bounding rectangle
}

void NodeGroup::removeNode(const QUuid& node_id)
{
  // remove node from list and update the bounding rectangle
}

