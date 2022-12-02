#include "UndoCommands.hpp"

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "Definitions.hpp"
#include "NodeGraphicsObject.hpp"

#include <QtCore/QJsonArray>
#include <QtWidgets/QGraphicsObject>

#include <typeinfo>


namespace QtNodes
{

DeleteCommand::
DeleteCommand(BasicGraphicsScene* scene)
  : _scene(scene)
{
  auto & graphModel = _scene->graphModel();

  QJsonArray connJsonArray;
  // Delete the selected connections first, ensuring that they won't be
  // automatically deleted when selected nodes are deleted (deleting a
  // node deletes some connections as well)
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
    {
      auto const& cid = c->connectionId();

      connJsonArray.append(toJson(cid));
    }
  }

  QJsonArray nodesJsonArray;
  // Delete the nodes; this will delete many of the connections.
  // Selected connections were already deleted prior to this loop,
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
    {
      // saving connections attached to the selected nodes
      for (auto const & cid : graphModel.allConnectionIds(n->nodeId()))
      {
        connJsonArray.append(toJson(cid));
      }

      nodesJsonArray.append(graphModel.saveNode(n->nodeId()));
    }
  }

  _sceneJson["nodes"] = nodesJsonArray;
  _sceneJson["connections"] = connJsonArray;
}


void
DeleteCommand::
undo()
{
  auto & graphModel = _scene->graphModel();

  QJsonArray nodesJsonArray = _sceneJson["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    QJsonObject obj = node.toObject();
    graphModel.loadNode(obj);
  }

  QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId = fromJson(connJson);

    // Restore the connection
    graphModel.addConnection(connId);
  }
}


void
DeleteCommand::
redo()
{
  auto & graphModel = _scene->graphModel();

  QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId = fromJson(connJson);

    graphModel.deleteConnection(connId);
  }


  QJsonArray nodesJsonArray = _sceneJson["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    QJsonObject nodeJson = node.toObject();
    graphModel.deleteNode(static_cast<NodeId>(nodeJson["id"].toInt()));
  }
}


//-------------------------------------


DuplicateCommand::
DuplicateCommand(BasicGraphicsScene* scene,
                 QPointF const & mouseScenePos)
  : _scene(scene)
  , _mouseScenePos(mouseScenePos)
{
  auto & graphModel = _scene->graphModel();

  std::unordered_set<NodeId> selectedNodes;

  QJsonArray nodesJsonArray;
  // Delete the nodes; this will delete many of the connections.
  // Selected connections were already deleted prior to this loop,
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
    {
      nodesJsonArray.append(graphModel.saveNode(n->nodeId()));

      selectedNodes.insert(n->nodeId());
    }
  }

  QJsonArray connJsonArray;
  // Delete the selected connections first, ensuring that they won't be
  // automatically deleted when selected nodes are deleted (deleting a
  // node deletes some connections as well)
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
    {
      auto const& cid = c->connectionId();

      if (selectedNodes.count(cid.outNodeId) > 0 &&
          selectedNodes.count(cid.inNodeId) > 0)
      {
        connJsonArray.append(toJson(cid));
      }
    }
  }


  _sceneJson["nodes"] = nodesJsonArray;
  _sceneJson["connections"] = connJsonArray;
}


void
DuplicateCommand::
undo()
{
  auto & graphModel = _scene->graphModel();

  QJsonArray connectionJsonArray = _newSceneJson["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId = fromJson(connJson);

    graphModel.deleteConnection(connId);
  }

  QJsonArray nodesJsonArray = _newSceneJson["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    QJsonObject nodeJson = node.toObject();
    graphModel.deleteNode(static_cast<NodeId>(nodeJson["id"].toInt()));
  }
}


void
DuplicateCommand::
redo()
{
  _scene->clearSelection();

  auto & graphModel = _scene->graphModel();

  std::unordered_map<NodeId, NodeId> mapNodeIds;

  QPointF averagePos;

  QJsonArray nodesJsonArray = _sceneJson["nodes"].toArray();

  // Cycle below replaces the NodeId with the new generated value
  // and computes an average position of the old selected node group

  QJsonArray newNodesJsonArray;
  for (QJsonValueRef node : nodesJsonArray)
  {
    QJsonObject nodeJson = node.toObject();

    NodeId oldNodeId = nodeJson["id"].toInt();

    averagePos +=
      QPointF(nodeJson["position"].toObject()["x"].toDouble(),
              nodeJson["position"].toObject()["y"].toDouble());

    NodeId newNodeId = graphModel.newNodeId();

    mapNodeIds[oldNodeId] = newNodeId;

    // Replace NodeId in json
    nodeJson["id"] = static_cast<qint64>(newNodeId);

    newNodesJsonArray.append(nodeJson);
  }

  averagePos /= static_cast<double>(nodesJsonArray.size());


  // The cycle below replaces old NodeIds in connections with the new values

  QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

  QJsonArray newConnJsonArray;
  for (QJsonValueRef connection : connectionJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId = fromJson(connJson);

    ConnectionId newConnId{mapNodeIds[connId.outNodeId],
                           connId.outPortIndex,
                           mapNodeIds[connId.inNodeId],
                           connId.inPortIndex};


    newConnJsonArray.append(toJson(newConnId));
  }

  // Cycle below offsets the new node group to the position of the mouse cursor
  QPointF const diff = _mouseScenePos - averagePos;

  for (QJsonValueRef node : newNodesJsonArray)
  {
    QJsonObject obj = node.toObject();
    NodeId const id = obj["id"].toInt();

    QPointF oldPos(obj["position"].toObject()["x"].toDouble(),
                   obj["position"].toObject()["y"].toDouble());

    oldPos += diff;

    QJsonObject posJson;
    posJson["x"] = oldPos.x();
    posJson["y"] = oldPos.y();
    obj["position"] = posJson;


    graphModel.loadNode(obj);

    _scene->nodeGraphicsObject(id)->setZValue(1.0);
  }

  for (QJsonValueRef connection : newConnJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId = fromJson(connJson);

    // Restore the connection
    graphModel.addConnection(connId);
  }


  _newSceneJson["nodes"] = newNodesJsonArray;
  _newSceneJson["connections"] = newConnJsonArray;
}


//-------------------------------------



DisconnectCommand::
DisconnectCommand(BasicGraphicsScene* scene,
                  ConnectionId const connId)
  : _scene(scene)
  , _connId(connId)
{
  //
}

void
DisconnectCommand::
undo()
{
  _scene->graphModel().addConnection(_connId);
}


void
DisconnectCommand::
redo()
{
  _scene->graphModel().deleteConnection(_connId);
}


//------


ConnectCommand::
ConnectCommand(BasicGraphicsScene* scene,
               ConnectionId const connId)
  : _scene(scene)
  , _connId(connId)
{
  //
}

void
ConnectCommand::
undo()
{
  _scene->graphModel().deleteConnection(_connId);
}


void
ConnectCommand::
redo()
{
  _scene->graphModel().addConnection(_connId);
}


//------


MoveNodeCommand::
MoveNodeCommand(BasicGraphicsScene* scene,
                NodeId const nodeId,
                QPointF const &diff)
  : _scene(scene)
  , _nodeId(nodeId)
  , _diff(diff)
{
}

void
MoveNodeCommand::
undo()
{
  auto oldPos = 
    _scene->graphModel().nodeData(_nodeId,
                                  NodeRole::Position).value<QPointF>();

  oldPos -= _diff;

  _scene->graphModel().setNodeData(_nodeId, NodeRole::Position, oldPos);
}


void
MoveNodeCommand::
redo()
{
  auto oldPos = 
    _scene->graphModel().nodeData(_nodeId,
                                  NodeRole::Position).value<QPointF>();

  oldPos += _diff;

  _scene->graphModel().setNodeData(_nodeId, NodeRole::Position, oldPos);
}


int
MoveNodeCommand::
id() const
{
  return static_cast<int>(typeid(MoveNodeCommand).hash_code());
}


bool
MoveNodeCommand::
mergeWith(QUndoCommand const *c) 
{
  auto mc = static_cast<MoveNodeCommand const*>(c);

  _diff += mc->_diff;

  return true;
}

//
}
