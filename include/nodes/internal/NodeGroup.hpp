#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <vector>

#include "GroupGraphicsObject.hpp"
#include "Node.hpp"
#include "Export.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes
{

class Node;
class GroupGraphicsObject;

class NODE_EDITOR_PUBLIC NodeGroup
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  NodeGroup(const std::vector<Node*>& nodes,
            const QString& name = QStringLiteral(""));

  virtual
  ~NodeGroup() override;

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &json) override;

public:

  QUuid
  id() const;

  GroupGraphicsObject const &
  groupGraphicsObject() const;

  GroupGraphicsObject &
  groupGraphicsObject();

  std::vector<Node*>&
  childNodes();

  QString const &
  name() const;

  void
  setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object);

  bool empty() const;

  static int groupCount();

public Q_SLOTS:

  void
  addNode(Node* node);

  void
  removeNode(Node* node);

private:
  // addressing
  QUuid _uid;

  QString _name;

  // data
  /** @todo check if the node's memory management should be done differently; this
   * seems really error-prone */
  std::vector<Node*> _childNodes;

  // painting
  std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;

  static int _groupCount;
};
}
