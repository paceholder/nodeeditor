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
class Connection;



class NODE_EDITOR_PUBLIC NodeGroup
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:
  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  NodeGroup(const std::vector<QUuid>& nodeKeys,
            const std::vector<QUuid>& connectionKeys,
            std::unordered_map<QUuid, UniqueNode>& nodes,
            std::unordered_map<QUuid, SharedConnection>& connections);

  virtual
  ~NodeGroup() override;

public:

  void saveGroupFile() const;

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

  std::unordered_map<QUuid, UniqueNode> const &
  childNodes() const;

  std::unordered_map<QUuid, SharedConnection> const &
  childConnections() const;

  bool locked() const;

  void
  setGraphicsObject(std::unique_ptr<GroupGraphicsObject>&& graphics_object);

  bool empty() const;

  void setSelected(bool selected);

  void lock(bool locked);

  void stealNodes(const std::vector<QUuid>& keys,
                  std::unordered_map<QUuid, UniqueNode>& nodes);

  void stealConnections(const std::vector<QUuid>& keys,
                        std::unordered_map<QUuid, SharedConnection>& connections);

public Q_SLOTS:

  void
  addNodeGraphicsObject(NodeGraphicsObject& ngo);

  void
  addNodeToGroup(Node* node);

  // this function has this return type to make use of C++17's extract method.
  std::unordered_map<QUuid, UniqueNode>::node_type
  removeNodeFromGroup(QUuid nodeID);

  //  void calculateArea();

private:
  // addressing
  QUuid _uid;

  QString _name;

  bool _locked;

  // data
  std::unordered_map<QUuid, UniqueNode> _childNodes{};
  std::unordered_map<QUuid, SharedConnection> _childConnections{};

  // painting
  std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;
};
}
