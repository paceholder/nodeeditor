#include "UndoCommands.hpp"

#include "Definitions.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
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

      connJsonArray.append(graphModel.saveConnection(cid));
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
        connJsonArray.append(graphModel.saveConnection(cid));
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
    graphModel.loadNode(node.toObject());
  }

  QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    graphModel.loadConnection(connection.toObject());
  }
}


void
DeleteCommand::
redo()
{
  auto & graphModel = _scene->graphModel();

  QJsonArray nodesJsonArray = _sceneJson["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    QJsonObject nodeJson = node.toObject();
    graphModel.deleteNode(static_cast<NodeId>(nodeJson["id"].toInt()));
  }

  QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    QJsonObject connJson = connection.toObject();

    ConnectionId connId{static_cast<NodeId>(connJson["outNodeId"].toInt()),
                        static_cast<PortIndex>(connJson["outPortIndex"].toInt()),
                        static_cast<NodeId>(connJson["intNodeId"].toInt()),
                        static_cast<PortIndex>(connJson["inPortIndex"].toInt())};

    graphModel.deleteConnection(connId);
  }
}




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
