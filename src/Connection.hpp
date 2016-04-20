#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "PortType.hpp"

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
  void setRequiredPort(PortType portType);
  PortType requiredPort() const;

  /// Returns address of Node for givend portType (NodeId, EndN)
  std::pair<QUuid, int> getAddress(PortType portType) const;
  void setAddress(PortType portType, PortAddress address);

public:

  /// When Connection initiates interaction
  bool tryConnectToNode(std::shared_ptr<Node> node, QPointF const& scenePoint);

  /// When Node initiates interaction
  void connectToNode(PortAddress const &address);

public:

  std::unique_ptr<ConnectionGraphicsObject> & getConnectionGraphicsObject() const;

  ConnectionState const & connectionState() const;
  ConnectionState& connectionState();

  ConnectionGeometry& connectionGeometry();

private:

  struct ConnectionImpl;

  std::unique_ptr<ConnectionImpl> _impl;
};
