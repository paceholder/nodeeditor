#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "EndType.hpp"

class Connection;
class ConnectionState;
class NodeGeometry;
class NodeGraphicsObject;
class NodeDataModel;
class NodeState;

class Node : public QObject
{
  Q_OBJECT

public:

  Node(std::unique_ptr<NodeDataModel> &&dataModel);

  ~Node();

public:

  QUuid id() const;

  void reactToPossibleConnection(EndType,
                                 QPointF const & scenePoint);

  void resetReactionToConnection();

  bool canConnect(ConnectionState const& connectionState,
                  QPointF const &scenePoint);

  std::pair<QUuid, int>
  connect(Connection const* connection, int hit);

  std::pair<QUuid, int>
  connect(Connection const* connection, QPointF const& scenePoint);

  void disconnect(Connection const* connection, EndType endType, int hit);

public:

  std::unique_ptr<NodeGraphicsObject> const &nodeGraphicsObject() const;
  std::unique_ptr<NodeGraphicsObject> & nodeGraphicsObject();

  NodeGeometry& nodeGeometry();
  NodeGeometry& nodeGeometry() const;

  NodeState const & nodeState() const;

  std::unique_ptr<NodeDataModel> const & nodeDataModel() const;


private:

  struct NodeImpl;

  std::unique_ptr<NodeImpl> _impl;
};
