#pragma once

#include "BasicGraphicsScene.hpp"
#include "DataFlowGraphModel.hpp"
#include "Export.hpp"


namespace QtNodes
{


/// @brief An advanced scene working with data-propagating graphs.
/**
 * The class represents a scene that existed in v2.x but built wit the
 * new model-view approach in mind.
 */
class NODE_EDITOR_PUBLIC DataFlowGraphicsScene
  : public BasicGraphicsScene
{
  Q_OBJECT
public:

  DataFlowGraphicsScene(DataFlowGraphModel &graphModel,
                        QObject * parent = nullptr);

  ~DataFlowGraphicsScene() = default;

public:
  std::vector<NodeId>
  selectedNodes() const;

public:
  QMenu *
  createSceneMenu(QPointF const scenePos) override;


public Q_SLOTS:
  void
  save() const;

  void
  load();

private:
  DataFlowGraphModel &_graphModel;
};

}
