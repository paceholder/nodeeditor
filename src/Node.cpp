#include "Node.hpp"

#include <QtCore/QObject>

#include <utility>
#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "Connection.hpp"

using QtNodes::Node;
using QtNodes::NodeDataModel;
using QtNodes::Connection;

Node::
Node(std::unique_ptr<NodeDataModel> && dataModel, QUuid const& id)
  : _uid(id),
    _nodeDataModel(std::move(dataModel))
  
{
  // propagate data: model => node
  connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated,
          this, &Node::onDataUpdated);

  _inConnections.resize(nodeDataModel()->nPorts(PortType::In));
  _outConnections.resize(nodeDataModel()->nPorts(PortType::Out));
}


Node::
~Node() = default;

QJsonObject
Node::
save() const
{
  QJsonObject nodeJson;

  nodeJson["id"] = id().toString();

  nodeJson["model"] = _nodeDataModel->save();

  QJsonObject obj;
  obj["x"] = _pos.x();
  obj["y"] = _pos.y();
  nodeJson["position"] = obj;

  return nodeJson;
}


void
Node::
restore(QJsonObject const& json)
{

  QJsonObject positionJson = json["position"].toObject();
  QPointF     point(positionJson["x"].toDouble(),
                    positionJson["y"].toDouble());
  setPosition(point);

  _nodeDataModel->restore(json["model"].toObject());
}


QUuid
Node::
id() const
{
  return _uid;
}

NodeDataModel*
Node::
nodeDataModel() const
{
  return _nodeDataModel.get();
}

QPointF
Node::
position() const
{
  return _pos;
}

void
Node::
setPosition(QPointF const& newPos) {
  _pos = newPos;

  emit positionChanged(newPos);
}

std::vector<Connection*> const&
Node::
connections(PortType pType, PortIndex idx) const
{
  Q_ASSERT(idx >= 0);
  Q_ASSERT(pType == PortType::In ? (size_t)idx < _inConnections.size() : (size_t)idx < _outConnections.size());

  return pType == PortType::In ? _inConnections[idx] : _outConnections[idx];
}

std::vector<Connection*> &
Node::
connections(PortType pType, PortIndex idx)
{
  Q_ASSERT(idx >= 0);
  Q_ASSERT(pType == PortType::In ? (size_t)idx < _inConnections.size() : (size_t)idx < _outConnections.size());

  return pType == PortType::In ? _inConnections[idx] : _outConnections[idx];
}

void
Node::
propagateData(std::shared_ptr<NodeData> nodeData,
              PortIndex inPortIndex) const
{
  _nodeDataModel->setInData(nodeData, inPortIndex);
}


void
Node::
onDataUpdated(PortIndex index)
{
  auto nodeData = _nodeDataModel->outData(index);
  auto& conns =
    connections(PortType::Out, index);

  for (auto const & c : conns)
    c->propagateData(nodeData);
}
