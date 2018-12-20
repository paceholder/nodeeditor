#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include "PortType.hpp"
#include "NodeData.hpp"

#include "Serializable.hpp"
#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "TypeConverter.hpp"
#include "QUuidStdHash.hpp"
#include "Export.hpp"
#include "memory.hpp"
#include "ConnectionID.hpp"

class QPointF;

namespace QtNodes
{

class Node;
class NodeData;
class ConnectionGraphicsObject;

/// Connection is a representation in DataFlowScene of a connection
/// It is part of the model, and not the rendering system.
/// This class is not to be used if you're implementing FlowScene model yourself
class NODE_EDITOR_PUBLIC Connection
  : public QObject
  , public Serializable
{

  Q_OBJECT

public:

  Connection(Node& nodeIn,
             PortIndex portIndexIn,
             Node& nodeOut,
             PortIndex portIndexOut,
             TypeConverter converter =
               TypeConverter{});

  Connection(const Connection&) = delete;
  Connection
  operator=(const Connection&) = delete;

  ~Connection();

public:

  QJsonObject
  save() const override;

public:

  ConnectionID
  id() const;

  Node*
  getNode(PortType portType) const;

private:

  Node*&
  getNodePtrRef(PortType portType);

public:

  PortIndex
  getPortIndex(PortType portType) const;

  NodeDataType
  dataType(PortType portType) const;

  void
  setTypeConverter(TypeConverter converter);

public: // data propagation

  void
  propagateData(std::shared_ptr<NodeData> nodeData) const;

  void
  propagateEmptyData() const;

private:

  QUuid _uid;

private:

  void
  setNodeToPort(Node& node,
                PortType portType,
                PortIndex portIndex);

  Node* _outNode = nullptr;
  Node* _inNode  = nullptr;

  PortIndex _outPortIndex;
  PortIndex _inPortIndex;

private:

  ConnectionState _connectionState;
  ConnectionGeometry _connectionGeometry;

  std::unique_ptr<ConnectionGraphicsObject>_connectionGraphicsObject;

  TypeConverter _converter;

signals:

  void
  updated(Connection& conn) const;
};
}
