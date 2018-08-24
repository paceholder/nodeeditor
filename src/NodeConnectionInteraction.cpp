#include "NodeConnectionInteraction.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"
#include "DataModelRegistry.hpp"
#include "FlowScene.hpp"

using QtNodes::NodeConnectionInteraction;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::FlowScene;
using QtNodes::NodeDataModel;
using QtNodes::TypeConverter;
using QtNodes::NodeIndex;

NodeConnectionInteraction::
NodeConnectionInteraction(NodeIndex const& node,
                          ConnectionGraphicsObject& connection)
  : _node(node)
  , _connection(&connection)
{}


bool
NodeConnectionInteraction::
canConnect(PortIndex &portIndex, bool& converted) const
{
  // 1) Connection requires a port

  PortType requiredPort = connectionRequiredPort();


  if (requiredPort == PortType::None)
  {
    return false;
  }

  // 1.5) Forbid connecting the node to itself
  NodeIndex node = _connection->node(oppositePort(requiredPort));

  if (node == _node)
    return false;

  // 2) connection point is on top of the node port

  QPointF connectionPoint = connectionEndScenePosition(requiredPort);

  portIndex = nodePortIndexUnderScenePoint(requiredPort,
                                           connectionPoint);

  if (portIndex == INVALID)
  {
    return false;
  }

  // 3) Node port is vacant

  // port should be empty
  if (!nodePortIsEmpty(requiredPort, portIndex))
    return false;

  // 4) Connection type equals node port type, or there is a registered type conversion that can translate between the two

  auto connectionDataType =
    _connection->dataType(oppositePort(requiredPort));

  auto const modelTarget             = _node.model();
  NodeDataType candidateNodeDataType = modelTarget->nodePortDataType(_node, portIndex, requiredPort);

  // if the types don't match, try a conversion
  if (connectionDataType.id != candidateNodeDataType.id)
  {
    converted = true;
    if (requiredPort == PortType::In)
    {
      return modelTarget->getTypeConvertable({connectionDataType, candidateNodeDataType});
    }
    else if (requiredPort == PortType::Out)
    {
      return modelTarget->getTypeConvertable({candidateNodeDataType, connectionDataType});
    }
    Q_UNREACHABLE();
  }

  converted = false;

  return true;
}


bool
NodeConnectionInteraction::
tryConnect() const
{
  // 1) Check conditions from 'canConnect'
  PortIndex portIndex = INVALID;

  bool converted;

  if (!canConnect(portIndex, converted))
  {
    return false;
  }

  auto requiredPort  = connectionRequiredPort();
  auto connectedPort = oppositePort(requiredPort);

  auto outNodePortIndex = _connection->portIndex(connectedPort);
  auto outNode          = _connection->node(connectedPort);

  auto model = _connection->flowScene().model();

  if (requiredPort == PortType::In)
  {
    return model->addConnection(outNode, outNodePortIndex, _node, portIndex);
  }
  return model->addConnection(_node, portIndex, outNode, outNodePortIndex);
}

// ------------------ util functions below

PortType
NodeConnectionInteraction::
connectionRequiredPort() const
{
  auto const &state = _connection->state();

  return state.requiredPort();
}


QPointF
NodeConnectionInteraction::
connectionEndScenePosition(PortType portType) const
{
  ConnectionGeometry& geometry = _connection->geometry();

  QPointF endPoint = geometry.getEndPoint(portType);

  return _connection->mapToScene(endPoint);
}


QPointF
NodeConnectionInteraction::
nodePortScenePosition(PortType portType, PortIndex portIndex) const
{

  NodeGraphicsObject const& ngo = *_connection->flowScene().nodeGraphicsObject(_node);

  NodeGeometry const &geom = ngo.geometry();

  QPointF p = geom.portScenePosition(portIndex, portType);

  return ngo.sceneTransform().map(p);
}


PortIndex
NodeConnectionInteraction::
nodePortIndexUnderScenePoint(PortType portType,
                             QPointF const & scenePoint) const
{
  NodeGraphicsObject const& ngo = *_connection->flowScene().nodeGraphicsObject(_node);
  NodeGeometry const &nodeGeom  = ngo.geometry();

  QTransform sceneTransform =
    ngo.sceneTransform();

  PortIndex portIndex = nodeGeom.checkHitScenePoint(portType,
                                                    scenePoint,
                                                    sceneTransform);
  return portIndex;
}


bool
NodeConnectionInteraction::
nodePortIsEmpty(PortType portType, PortIndex portIndex) const
{
  NodeState const & nodeState = _connection->flowScene().nodeGraphicsObject(_node)->nodeState();

  auto const & entries = nodeState.getEntries(portType);

  if (entries[portIndex].empty())
    return true;

  const auto outPolicy = _node.model()->nodePortConnectionPolicy(_node, portIndex, portType);
  return outPolicy == ConnectionPolicy::Many;
}
