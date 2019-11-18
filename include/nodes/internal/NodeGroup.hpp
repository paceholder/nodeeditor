#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <vector>

#include "GroupGraphicsObject.hpp"
#include "Export.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes
{

class Node;

class NODE_EDITOR_PUBLIC NodeGroup
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:
  NodeGroup(std::vector<std::unique_ptr<Node>>&& nodes);

public:
  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &json) override;

public:
  QUuid
  id() const;

public Q_SLOTS:
  void
  addNode(const QUuid& node_id);

  void
  removeNode(const QUuid& node_id);

  void
  setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object);

  //  void calculateArea();

private:
  // addressing
  QUuid _uid;

  // data
  std::vector<std::unique_ptr<Node>> _childNodes;

  // painting
  std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;
};
}
