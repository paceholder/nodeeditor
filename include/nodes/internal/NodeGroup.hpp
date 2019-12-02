#pragma once

#include <QtCore/QUuid>
#include <QtCore/QString>

#include <vector>

#include "Node.hpp"

namespace QtNodes
{

class NodeGroup
{

public:
  NodeGroup();
  explicit NodeGroup(const std::vector<QUuid>& childNodes);
  NodeGroup(const QString& name);
  NodeGroup(const std::vector<QUuid>& childNodes, const QString& name);

  QUuid const&
  id() const;

  QString
  name() const;

  void
  setName(const QString& name);

private:
  QUuid _uid;
  QString _name {};
  std::vector<QUuid> _childNodes {};
};

}
