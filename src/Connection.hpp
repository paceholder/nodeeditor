#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "EndType.hpp"

class Node;
class ConnectionGraphicsObject;
class ConnectionGeometry;
class QPointF;

//------------------------------------------------------------------------------

class Connection : QObject
{
  Q_OBJECT

public:

  Connection();
  ~Connection();

  QUuid id() const;

  void setDraggingEnd(EndType dragging);
  EndType draggingEnd() const;

  std::pair<QUuid, int> getAddress(EndType endType) const;
  void setAddress(EndType endType, std::pair<QUuid, int> address);

public:

  /// Connection initiates interaction
  void tryConnectToNode(Node* node, QPointF const& scenePoint);

  /// Node initiates
  void connectToNode(std::pair<QUuid, int> const &address,
                     QPointF const& scenePoint);

  ConnectionGraphicsObject* getConnectionGraphicsObject() const;

  ConnectionGeometry& connectionGeometry();

private:

  class ConnectionImpl;

  std::unique_ptr<ConnectionImpl> _impl;
};

#endif // CONNECTION_H
