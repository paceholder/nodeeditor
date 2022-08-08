#pragma once

#include "Definitions.hpp"

#include <QtGui/QUndoCommand>
#include <QtCore/QPointF>
#include <QtCore/QJsonObject>

namespace QtNodes
{

class BasicGraphicsScene;


class DeleteCommand : public QUndoCommand
{
public:
  DeleteCommand(BasicGraphicsScene* scene);

  void undo() override;
  void redo() override;

private:
  BasicGraphicsScene* _scene;

  QJsonObject _sceneJson;
};


class DisconnectCommand : public QUndoCommand
{
public:
  DisconnectCommand(BasicGraphicsScene* scene,
                    ConnectionId const);

  void undo() override;
  void redo() override;

private:
  BasicGraphicsScene* _scene;

  ConnectionId _connId;
};


class ConnectCommand : public QUndoCommand
{
public:
  ConnectCommand(BasicGraphicsScene* scene,
                 ConnectionId const);

  void undo() override;
  void redo() override;

private:
  BasicGraphicsScene* _scene;

  ConnectionId _connId;
};


class MoveNodeCommand : public QUndoCommand
{
public:
  MoveNodeCommand(BasicGraphicsScene* scene,
                  NodeId const nodeId,
                  QPointF const &diff);

  void undo() override;
  void redo() override;

  int id() const override;

  bool mergeWith(QUndoCommand const *c) override;

private:
  BasicGraphicsScene* _scene;
  NodeId _nodeId;
  QPointF _diff;
};


}
