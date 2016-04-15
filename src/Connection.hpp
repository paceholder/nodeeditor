#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "EndType.hpp"

class Node;
class ConnectionGraphicsObject;
class ConnectionState;
class ConnectionGeometry;
class QPointF;

//------------------------------------------------------------------------------

///
class Connection
{
public:

  Connection();
  ~Connection();

  QUuid id() const;

  /// Remembers the end being dragged.
  /// Invalidates Node address.
  /// Grabs mouse.
  void setDraggingEnd(EndType dragging);
  EndType draggingEnd() const;

  /// Returns address of Node for givend endType (NodeId, EndN)
  std::pair<QUuid, int> getAddress(EndType endType) const;
  void setAddress(EndType endType, std::pair<QUuid, int> address);

public:

  /// When Connection initiates interaction
  bool tryConnectToNode(Node* node, QPointF const& scenePoint);

  /// When Node initiates interaction
  void connectToNode(std::pair<QUuid, int> const &address);

public:

  ConnectionGraphicsObject* getConnectionGraphicsObject() const;

  ConnectionState const & connectionState() const;
  ConnectionState& connectionState();

  ConnectionGeometry& connectionGeometry();

private:

  class ConnectionImpl;

  std::unique_ptr<ConnectionImpl> _impl;
};
