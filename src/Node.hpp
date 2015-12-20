#ifndef NODE_HPP
#define NODE_HPP

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "EndType.hpp"

class Connection;
class NodeGeometry;
class NodeGraphicsObject;

class Node : public QObject
{
  Q_OBJECT

public:

  Node();
  ~Node();

public:

  QUuid const id() const;

  void reactToPossibleConnection(EndType,
                                 QPointF const & scenePoint);

  /// Updates graphics
  void update();

  bool canConnect(EndType draggingEnd,
                  QPointF const &scenePoint);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          int hit);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          QPointF const& scenePoint);



  void disconnect(Connection const* connection,
                  EndType endType,
                  int hit);

  NodeGraphicsObject const* nodeGraphicsObject() const;

  NodeGeometry& nodeGeometry();
  NodeGeometry const& nodeGeometry() const;

private:

  class NodeImpl;

  std::unique_ptr<NodeImpl> _impl;
};

#endif // NODE_HPP
