#pragma once

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "DataFlowGraphModel.hpp"
#include "Export.hpp"
#include "NodeConnectionInteraction.hpp"

namespace QtNodes {

/**
 * @brief An advanced scene working with data-propagating graphs.
 *
 * The class represents a scene that existed in v2.x but built wit the
 * new model-view approach in mind.
 */
class NODE_EDITOR_PUBLIC DataFlowGraphicsScene : public BasicGraphicsScene
{
    Q_OBJECT
public:
    DataFlowGraphicsScene(DataFlowGraphModel &graphModel, QObject *parent = nullptr);
    ~DataFlowGraphicsScene() = default;

public:
    std::vector<NodeId> selectedNodes() const;
    QMenu *createSceneMenu(QPointF const scenePos) override;
    void updateConnectionGraphics(const std::unordered_set<ConnectionId> &connections, bool state)
    {
        for (auto const &c : connections) {
            if (auto *cgo = connectionGraphicsObject(c)) {
                // NodeEditor3 does not expose connection geometry
                // directly. Update the graphics object to reflect
                // the frozen state if available.
                cgo->connectionState().setFrozen(state);
                cgo->update();
            }
        }
    }

public Q_SLOTS:
    bool save() const;
    bool load();

Q_SIGNALS:
    void sceneLoaded();

private:
    DataFlowGraphModel &_graphModel;
};

} // namespace QtNodes
