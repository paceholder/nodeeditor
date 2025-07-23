#pragma once

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
class GraphEditorScene : public DataFlowGraphicsScene
{
public:
    GraphEditorScene(DataFlowGraphModel &model)
        : DataFlowGraphicsScene(model)
    {}

    QMenu *createSceneMenu(QPointF const scenePos) override { return nullptr; }
};