#include "Node.hpp"

#include <QtCore/QObject>

#include <utility>
#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

using QtNodes::Node;
using QtNodes::NodeGeometry;
using QtNodes::NodeState;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeGraphicsObject;
using QtNodes::PortIndex;
using QtNodes::PortType;

Node::
Node(std::unique_ptr<NodeDataModel> && dataModel)
  : _uid(QUuid::createUuid())
  , _nodeDataModel(std::move(dataModel))
  , _nodeState(_nodeDataModel)
  , _nodeGeometry(_nodeDataModel)
  , _nodeGraphicsObject(nullptr)
{
  _nodeGeometry.recalculateSize();

  // propagate data: model => node
  connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated,
      this, &Node::onDataUpdated);
  connect(_nodeDataModel.get(), &NodeDataModel::portAdded, this,
      [this](PortType type, PortIndex index) { updatePortChange(type, index, PortAdded); });
  connect(_nodeDataModel.get(), &NodeDataModel::portRemoved, this,
      [this](PortType type, PortIndex index) { updatePortChange(type, index, PortRemoved); });;
}


Node::
~Node() = default;

QJsonObject
Node::
save() const
{
  QJsonObject nodeJson;

  nodeJson["id"] = _uid.toString();

  nodeJson["model"] = _nodeDataModel->save();

  QJsonObject obj;
  obj["x"] = _nodeGraphicsObject->pos().x();
  obj["y"] = _nodeGraphicsObject->pos().y();
  nodeJson["position"] = obj;
  nodeJson["in"]  = (int)_nodeDataModel->nPorts(PortType::In);
  nodeJson["out"] = (int)_nodeDataModel->nPorts(PortType::Out);

  return nodeJson;
}


void
Node::
restore(QJsonObject const& json)
{
  _uid = QUuid(json["id"].toString());

  QJsonObject positionJson = json["position"].toObject();
  QPointF     point(positionJson["x"].toDouble(),
                    positionJson["y"].toDouble());
  _nodeGraphicsObject->setPos(point);

  _nodeDataModel->restore(json["model"].toObject());

  if(json.contains("in"))
    _nodeState._inConnections.resize(json["in"].toInt());
  if(json.contains("out"))
    _nodeState._outConnections.resize(json["out"].toInt());
}


QUuid
Node::
id() const
{
  return _uid;
}


void
Node::
reactToPossibleConnection(PortType reactingPortType,
                          NodeDataType const &reactingDataType,
                          QPointF const &scenePoint)
{
  QTransform const t = _nodeGraphicsObject->sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  _nodeGeometry.setDraggingPosition(p);

  _nodeGraphicsObject->update();

  _nodeState.setReaction(NodeState::REACTING,
                         reactingPortType,
                         reactingDataType);
}


void
Node::
resetReactionToConnection()
{
  _nodeState.setReaction(NodeState::NOT_REACTING);
  _nodeGraphicsObject->update();
}


NodeGraphicsObject const &
Node::
nodeGraphicsObject() const
{
  return *_nodeGraphicsObject.get();
}


NodeGraphicsObject &
Node::
nodeGraphicsObject()
{
  return *_nodeGraphicsObject.get();
}


void
Node::
setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics)
{
  _nodeGraphicsObject = std::move(graphics);

  _nodeGeometry.recalculateSize();
}


NodeGeometry&
Node::
nodeGeometry()
{
  return _nodeGeometry;
}


NodeGeometry const&
Node::
nodeGeometry() const
{
  return _nodeGeometry;
}


NodeState const &
Node::
nodeState() const
{
  return _nodeState;
}


NodeState &
Node::
nodeState()
{
  return _nodeState;
}


NodeDataModel*
Node::
nodeDataModel() const
{
  return _nodeDataModel.get();
}


void
Node::
propagateData(std::shared_ptr<NodeData> nodeData,
              PortIndex inPortIndex,
              const QUuid& connectionId) const
{
  _nodeDataModel->setInData(std::move(nodeData), inPortIndex, connectionId);

  recalculateVisuals();
}


void
Node::
onDataUpdated(PortIndex index)
{
  auto nodeData = _nodeDataModel->outData(index);

  auto connections =
    _nodeState.connections(PortType::Out, index);

  for (auto const & c : connections)
    c.second->propagateData(nodeData);
}

void
Node::
updatePortChange(PortType portType, PortIndex portIndex, bool portAdded)
{
  Q_UNUSED(portIndex);
  std::vector<NodeState::ConnectionPtrSet>* connections{ nullptr };

  switch (portType)
  {
  case PortType::In:
      _nodeGeometry._nSources = _nodeDataModel->nPorts(portType);
      connections = &_nodeState._inConnections;
      break;
  case PortType::Out:
      _nodeGeometry._nSinks = _nodeDataModel->nPorts(portType);
      connections = &_nodeState._outConnections;
      break;
  default:
      throw std::invalid_argument("Invalid PortType in Node::onPortAdded");
  }
  
  const int connectionPosDiff{ portAdded ? 1 : -1 };

  for (auto it = connections->begin() + portIndex; it != connections->end(); ++it)
  {
    for (auto&& item : *it)
    {
      auto* connection = item.second;
      connection->setNodeToPort(
        *connection->getNode(portType), 
        portType, 
        connection->getPortIndex(portType) + connectionPosDiff);
    }
  }

  if (portAdded)
  {
      connections->emplace(connections->begin() + portIndex);
  }
  else
  {
      // \todo Remove the lost connections.
      connections->erase(connections->begin() + portIndex);
  }
  assert(_nodeDataModel->nPorts(portType) == connections->size());

  recalculateVisuals();
}

void
Node::
recalculateVisuals() const
{
  //Recalculate the nodes visuals. A data change can result in the node taking more space than before, so this forces a recalculate+repaint on the affected node
  _nodeGraphicsObject->setGeometryChanged();
  _nodeGeometry.recalculateSize();
  _nodeGraphicsObject->update();
  _nodeGraphicsObject->moveConnections();
}
