#include "Node.hpp"

#include <QtCore/QObject>

#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

//------------------------------------------------------------------------------

Node::
Node(std::unique_ptr<NodeDataModel> && dataModel)
  : _id(QUuid::createUuid())
  , _nodeDataModel(std::move(dataModel))
  , _nodeState(_nodeDataModel)
  , _nodeGeometry(_nodeDataModel)
  , _nodeGraphicsObject(nullptr)
{
  _nodeGeometry.recalculateSize();

  // propagate data: model => node
  connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated,
          this, &Node::onDataUpdated);
}


Node::
~Node()
{
  std::cout << "Node destructor" << std::endl;
}


QUuid
Node::
id() const
{
  return _id;
}


void
Node::
reactToPossibleConnection(PortType,
                          QPointF const &scenePoint)
{
  QTransform const t = _nodeGraphicsObject->sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  _nodeGeometry.setDraggingPosition(p);

  _nodeGraphicsObject->update();

  _nodeState.setReaction(NodeState::REACTING);
}


void
Node::
resetReactionToConnection()
{
  _nodeState.setReaction(NodeState::NOT_REACTING);
  _nodeGraphicsObject->update();
}


std::unique_ptr<NodeGraphicsObject> const &
Node::
nodeGraphicsObject() const
{
  return _nodeGraphicsObject;
}


std::unique_ptr<NodeGraphicsObject> &
Node::
nodeGraphicsObject()
{
  return _nodeGraphicsObject;
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


std::unique_ptr<NodeDataModel> const &
Node::
nodeDataModel() const
{
  return _nodeDataModel;
}


void
Node::
propagateData(std::shared_ptr<NodeData> nodeData,
              PortIndex inPortIndex) const
{
  _nodeDataModel->setInData(nodeData, inPortIndex);

  _nodeGeometry.recalculateSize();
  _nodeGraphicsObject->setGeometryChanged();
  _nodeGraphicsObject->update();
}


void
Node::
onDataUpdated(PortIndex index)
{
  auto nodeData = _nodeDataModel->outData(index);

  auto connection = _nodeState.connection(PortType::OUT, index);

  if (nodeData && connection)
    connection->propagateData(nodeData);
}
