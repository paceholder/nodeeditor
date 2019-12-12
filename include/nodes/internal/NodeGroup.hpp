#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QByteArray>

#include <vector>

#include "GroupGraphicsObject.hpp"
#include "Node.hpp"
#include "Export.hpp"
#include "memory.hpp"
#include "Connection.hpp"

namespace QtNodes
{

class Node;
class GroupGraphicsObject;

class NODE_EDITOR_PUBLIC NodeGroup
  : public QObject
{
  Q_OBJECT

public:

  NodeGroup(const std::vector<Node*>& nodes,
            const QString& name = QStringLiteral(""));

  virtual
  ~NodeGroup() override;

public:

  QByteArray
  saveToFile() const;

  QUuid
  id() const;

  GroupGraphicsObject const &
  groupGraphicsObject() const;

  GroupGraphicsObject &
  groupGraphicsObject();

  std::vector<Node*>&
  childNodes();

  std::vector<QUuid>
  nodeIDs() const;

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

  QString _name;

  // addressing
  QUuid _uid;


  // data
  std::vector<Node*> _childNodes;

  // painting
  std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;

  static int _groupCount;
};
}
