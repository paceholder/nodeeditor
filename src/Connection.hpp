#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "PortType.hpp"
#include "NodeData.hpp"

#include "Serializable.hpp"
#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"

class Node;
class NodeData;
class ConnectionGraphicsObject;
class QPointF;

//------------------------------------------------------------------------------

///
class Connection : public Serializable
{
public:

  /// New Connection is attached to the port of the given Node.
  /// The port has parameters (portType, portIndex).
  /// The opposite connection end will require anothre port.
  Connection(PortType portType,
             std::shared_ptr<Node> node,
             PortIndex portIndex);

  Connection(std::shared_ptr<Node> nodeIn,
             PortIndex portIndexIn,
             std::shared_ptr<Node> nodeOut,
             PortIndex portIndexOut);

  ~Connection();

public:

  void
  save(Properties &p) const override;

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
  setNodeToPort(std::shared_ptr<Node> node,
                PortType portType,
                PortIndex portIndex);

public:

  std::unique_ptr<ConnectionGraphicsObject> const&
  getConnectionGraphicsObject() const;

  ConnectionState const &
  connectionState() const;
  ConnectionState&
  connectionState();

  ConnectionGeometry&
  connectionGeometry();

  std::weak_ptr<Node> const &
  getNode(PortType portType) const;
  std::weak_ptr<Node> &
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

  std::weak_ptr<Node> _outNode;
  std::weak_ptr<Node> _inNode;

  PortIndex _outPortIndex;
  PortIndex _inPortIndex;

private:

  ConnectionState    _connectionState;
  ConnectionGeometry _connectionGeometry;

  std::unique_ptr<ConnectionGraphicsObject> _connectionGraphicsObject;
};
