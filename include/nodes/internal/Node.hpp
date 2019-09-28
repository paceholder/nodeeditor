#pragma once


#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtCore/QJsonObject>

#include "PortType.hpp"

#include "Export.hpp"
#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeData.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes
{

class Connection;
class ConnectionState;
class NodeGraphicsObject;
class NodeDataModel;

class NODE_EDITOR_PUBLIC Node
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeDataModel> && dataModel);

  virtual
  ~Node();

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &json) override;

public:

  QUuid
  id() const;

  void reactToPossibleConnection(PortType,
                                 NodeDataType const &,
                                 QPointF const & scenePoint);

  void
  resetReactionToConnection();

public:

  NodeGraphicsObject const &
  nodeGraphicsObject() const;

  NodeGraphicsObject &
  nodeGraphicsObject();

  void
  setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

  NodeGeometry&
  nodeGeometry();

  NodeGeometry const&
  nodeGeometry() const;

  NodeState const &
  nodeState() const;

  NodeState &
  nodeState();

  NodeDataModel*
  nodeDataModel() const;

public Q_SLOTS: // data propagation

  /// Propagates incoming data to the underlying model.
  void
  propagateData(std::shared_ptr<NodeData> nodeData,
                PortIndex inPortIndex) const;

  /// Fetches data from model's OUT #index port
  /// and propagates it to the connection
  void
  onDataUpdated(PortIndex index);

  /// update the graphic part if the size of the embeddedwidget changes
  void
  onNodeSizeUpdated();

  /// Reallocate NodeState's connection sets to account for the new number of
  /// input/output ports
  /// NB: There is no general way of knowing how to maintain connections when
  /// port count changes, especially when the removed ones are not the last
  /// one, resulting in port shift that may plug connections of the wrong data
  /// type. For now, the best thing to do is to first remove all connections,
  /// change port count and then rebuild the connections appropriately, all of
  /// this from the node's data model. A more generic solution would be to
  /// split this port into insertPorts(row, count) and removePorts(row, count)
  void
  onPortCountChanged();

Q_SIGNALS:
  /// Ask flow scene to remove this connection
  void
  killConnection(Connection& connection);

private:
  /// Recalculate the nodes visuals. A data change can result in the node
  /// taking more space than before, so this forces a recalculate+repaint on
  /// the affected node
  void recalculateVisuals() const;

private:

  // addressing

  QUuid _uid;

  // data

  std::unique_ptr<NodeDataModel> _nodeDataModel;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
};
}
