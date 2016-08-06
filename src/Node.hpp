#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "PortType.hpp"

#include "NodeState.hpp"
#include "NodeGeometry.hpp"

class Connection;
class ConnectionState;
class NodeGraphicsObject;
class NodeDataModel;

class Node : public QObject
{
  Q_OBJECT

public:

  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeDataModel> &&dataModel);

  ~Node();

public:

  QUuid id() const;

  void reactToPossibleConnection(PortType,
                                 QPointF const & scenePoint);

  void resetReactionToConnection();

public:

  std::unique_ptr<NodeGraphicsObject> const& nodeGraphicsObject() const;
  std::unique_ptr<NodeGraphicsObject>& nodeGraphicsObject();

  void setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

  NodeGeometry& nodeGeometry();
  NodeGeometry const& nodeGeometry() const;

  NodeState const & nodeState() const;
  NodeState & nodeState();

  std::unique_ptr<NodeDataModel> const & nodeDataModel() const;

private:

  // addressing

  QUuid _id;

  // data

  std::unique_ptr<NodeDataModel> _nodeDataModel;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
};

//Q_DECLARE_METATYPE(std::shared_ptr<Node>);
//qRegisterMetaType<std::shared_ptr<Node>>();
