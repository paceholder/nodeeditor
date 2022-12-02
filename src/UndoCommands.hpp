#pragma once

#include "Definitions.hpp"

#include <QUndoCommand>
#include <QtCore/QPointF>
#include <QtCore/QJsonObject>

namespace QtNodes
{

class BasicGraphicsScene;


/**
 * Selected scene objects are serialized and then removed from the scene.
 * The deleted elements could be restored in `undo`.
 */
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


/**
 * A command used in `GraphicsView` when user duplicates the selected objects by
 * using Ctrl+D key combination.
 */
class DuplicateCommand : public QUndoCommand
{
public:
  DuplicateCommand(BasicGraphicsScene* scene,
                   QPointF const & mouseScenePos);

  /**
   * Uses the stored `_newSceneJson` variable with the serialized duplicates to
   * delet nodes and connections.
   */
  void undo() override;

  /**
   * Inserting duplicates is done via serializing the current scene selection and
   * then de-serirializing with on-the-fly substitution of newly generated
   * NodeIds for the inserted objects.
   *
   * A the same time all the new objects are stored in `_newSceneJson` in order
   * to be able to undo the duplication.
   */
  void redo() override;
private:
  BasicGraphicsScene* _scene;
  QPointF const & _mouseScenePos;
  QJsonObject _sceneJson;
  QJsonObject _newSceneJson;
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

  /**
   * A command ID is used in command compression. It must be an integer unique to
   * this command's class, or -1 if the command doesn't support compression.
   */
  int id() const override;

  /**
   * Several sequential movements could be merged into one command.
   */
  bool mergeWith(QUndoCommand const *c) override;

private:
  BasicGraphicsScene* _scene;
  NodeId _nodeId;
  QPointF _diff;
};


}
