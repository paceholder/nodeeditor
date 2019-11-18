#include "NodeGroup.hpp"

using QtNodes::NodeGroup;

NodeGroup::
NodeGroup(std::vector<std::unique_ptr<Node>>&& nodes)
  : _uid(QUuid::createUuid())
  , _childNodes(std::move(nodes))
  , _groupGraphicsObject(nullptr)
{}

QJsonObject
NodeGroup::
save() const
{}

void
NodeGroup::
restore(const QJsonObject& json)
{}

QUuid
NodeGroup::
id() const
{
  return _uid;
}

void NodeGroup::addNode(const QUuid& node_id) {}

void NodeGroup::removeNode(const QUuid& node_id) {}

void NodeGroup::setGraphicsObject(
    std::unique_ptr<GroupGraphicsObject>&& graphics_object) {
  _groupGraphicsObject = std::move(graphics_object);
}
