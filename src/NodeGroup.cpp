#include "NodeGroup.hpp"

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

void
NodeGroup::
setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object) {
  _groupGraphicsObject = std::move(graphics_object);
}

void
NodeGroup::
addNode(const QUuid& node_id)
{
}

void NodeGroup::removeNode(const QUuid& node_id)
{
}

