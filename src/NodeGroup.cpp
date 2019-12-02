#include "NodeGroup.hpp"

using QtNodes::NodeGroup;

NodeGroup::
NodeGroup()
  : _uid(QUuid::createUuid())
{}

NodeGroup::
NodeGroup(const std::vector<QUuid>& childNodes)
  : NodeGroup()
{
  _childNodes.reserve(childNodes.size());
  std::copy(childNodes.begin(), childNodes.end(), _childNodes.begin());
}

NodeGroup::
NodeGroup(const QString& name)
  : NodeGroup()
{
  _name = name;
}

NodeGroup::
NodeGroup(const std::vector<QUuid>& childNodes, const QString& name)
  : NodeGroup(childNodes)
{
  _name = name;
}

QUuid const &
NodeGroup::id() const
{
  return _uid;
}

QString
NodeGroup::
name() const
{
  return _name;
}

void
NodeGroup::
setName(const QString& name)
{
  _name = name;
}
