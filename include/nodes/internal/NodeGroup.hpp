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

  NodeGroup(std::vector<Node*>&& nodes);

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

  std::vector<Node*> const
  childNodes() const;

  bool locked() const;

  void
  setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object);

  bool empty() const;

  void setSelected(bool selected);

  void lock(bool locked);

public Q_SLOTS:

  void
  addNodeGraphicsObject(NodeGraphicsObject& ngo);

  void
  addNode(Node* node);

  void
  removeNode(Node* node);

  //  void calculateArea();

private:
  // addressing
  QUuid _uid;

  QString _name;

  bool _locked;

  // data
  /** @todo check if the node's memory management should be done differently; this
   * seems really error-prone */
  std::vector<Node*> _childNodes;

  // painting
  std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;
};
}
