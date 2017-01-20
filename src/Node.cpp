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


void
Node::
save(Properties &p) const
{
  // save unique object id
  p.put("id", _id);

  // save data model name
  _nodeDataModel->save(p);

  // save node graphics position
  p.put("position", _nodeGraphicsObject->pos());
}


void
Node::
restore(Properties const &p)
{
  p.get("id", &_id);

  QPointF point;
  p.get("position", &point);
  _nodeGraphicsObject->setPos(point );

  _nodeDataModel->restore(p);
}


QUuid
Node::
id() const
{
  return _id;
}


void
Node::
reactToPossibleConnection(PortType reactingPortType,

                          NodeDataType reactingDataType,
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
              PortIndex inPortIndex) const
{
  _nodeDataModel->setInData(nodeData, inPortIndex);
  
  //Recalculate the nodes visuals. A data change can result in the node taking more space than before, so this forces a recalculate+repaint on the affected node
  _nodeGraphicsObject->setGeometryChanged();
  _nodeGeometry.recalculateSize();
  _nodeGraphicsObject->update();
  _nodeGraphicsObject->moveConnections();
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
