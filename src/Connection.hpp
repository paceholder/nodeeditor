#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QVariant>

#include "PortType.hpp"
#include "NodeData.hpp"

#include "Serializable.hpp"
#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "QUuidStdHash.hpp"
#include "Export.hpp"

class QPointF;

namespace QtNodes
{

class Node;
class NodeData;
class ConnectionGraphicsObject;

///
class NODE_EDITOR_PUBLIC Connection
  : public QObject
  , public Serializable
{

  Q_OBJECT

public:

  /// New Connection is attached to the port of the given Node.
  /// The port has parameters (portType, portIndex).
  /// The opposite connection end will require anothre port.
  Connection(PortType portType,
             Node& node,
             PortIndex portIndex);

  Connection(Node& nodeIn,
             PortIndex portIndexIn,
             Node& nodeOut,
             PortIndex portIndexOut);

  Connection(const Connection&) = delete;
  Connection operator=(const Connection&) = delete;

  ~Connection();

public:

  QJsonObject
  save() const override;

public:

  QUuid
  id() const;

  /// Remembers the end being dragged.
  /// Invalidates Node address.
  /// Grabs mouse.
  void
  setRequiredPort(PortType portType);
  PortType
  requiredPort() const;

  void
  setGraphicsObject(std::unique_ptr<ConnectionGraphicsObject>&& graphics);

  /// Assigns a node to the required port.
  /// It is assumed that there is a required port, no extra checks
  void
  setNodeToPort(Node& node,
                PortType portType,
                PortIndex portIndex);

  void
  removeFromNodes() const;

public:

  ConnectionGraphicsObject&
  getConnectionGraphicsObject() const;

  ConnectionState const &
  connectionState() const;
  ConnectionState&
  connectionState();

  ConnectionGeometry&
  connectionGeometry();

  ConnectionGeometry const&
  connectionGeometry() const;

  Node*
  getNode(PortType portType) const;

  Node*&
  getNode(PortType portType);

  PortIndex
  getPortIndex(PortType portType) const;

  void
  clearNode(PortType portType);

  NodeDataType
  dataType() const;

public: // data propagation

  void
  propagateData(std::shared_ptr<NodeData> nodeData) const;
  void
  propagateEmptyData() const;

private:

  QUuid _id;

private:

  Node* _outNode = nullptr;
  Node* _inNode  = nullptr;

  PortIndex _outPortIndex;
  PortIndex _inPortIndex;

private:

  ConnectionState    _connectionState;
  ConnectionGeometry _connectionGeometry;

  std::unique_ptr<ConnectionGraphicsObject> _connectionGraphicsObject;

signals:
  void
  updated(Connection& conn) const;
};
}
