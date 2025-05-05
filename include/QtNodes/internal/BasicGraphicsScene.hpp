#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "ConnectionIdHash.hpp"
#include "Definitions.hpp"
#include "Export.hpp"

#include "QUuidStdHash.hpp"

class QUndoStack;

namespace QtNodes {

class AbstractConnectionPainter;
class AbstractGraphModel;
class AbstractNodePainter;
class ConnectionGraphicsObject;
class NodeGraphicsObject;
class NodeStyle;

/// An instance of QGraphicsScene, holds connections and nodes.
class NODE_EDITOR_PUBLIC BasicGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    BasicGraphicsScene(AbstractGraphModel &graphModel, QObject *parent = nullptr);

    // Scenes without models are not supported
    BasicGraphicsScene() = delete;

    ~BasicGraphicsScene();

public:
    /// @returns associated AbstractGraphModel.
    AbstractGraphModel const &graphModel() const;

    AbstractGraphModel &graphModel();

    AbstractNodeGeometry const &nodeGeometry() const;

    AbstractNodeGeometry &nodeGeometry();

    AbstractNodePainter &nodePainter();

    AbstractConnectionPainter &connectionPainter();

    void setNodePainter(std::unique_ptr<AbstractNodePainter> newPainter);

    void setConnectionPainter(std::unique_ptr<AbstractConnectionPainter> newPainter);

    QUndoStack &undoStack();

public:
    /// Creates a "draft" instance of ConnectionGraphicsObject.
    /**
   * The scene caches a "draft" connection which has one loose end.
   * After attachment the "draft" instance is deleted and instead a
   * normal "full" connection is created.
   * Function @returns the "draft" instance for further geometry
   * manipulations.
   */
    std::unique_ptr<ConnectionGraphicsObject> const &makeDraftConnection(
        ConnectionId const newConnectionId);

    /// Deletes "draft" connection.
    /**
   * The function is called when user releases the mouse button during
   * the construction of the new connection without attaching it to any
   * node.
   */
    void resetDraftConnection();

    /// Deletes all the nodes. Connections are removed automatically.
    void clearScene();

public:
    /// @returns NodeGraphicsObject associated with the given nodeId.
    /**
   * @returns nullptr when the object is not found.
   */
    NodeGraphicsObject *nodeGraphicsObject(NodeId nodeId);

    /// @returns ConnectionGraphicsObject corresponding to `connectionId`.
    /**
   * @returns `nullptr` when the object is not found.
   */
    ConnectionGraphicsObject *connectionGraphicsObject(ConnectionId connectionId);

    Qt::Orientation orientation() const { return _orientation; }

    void setOrientation(Qt::Orientation const orientation);

public:
    /// Can @return an instance of the scene context menu in subclass.
    /**
   * Default implementation returns `nullptr`.
   */
    virtual QMenu *createSceneMenu(QPointF const scenePos);

Q_SIGNALS:
    void modified(BasicGraphicsScene *);

    void nodeMoved(NodeId const nodeId, QPointF const &newLocation);

    void nodeClicked(NodeId const nodeId);

    void nodeSelected(NodeId const nodeId);

    void nodeDoubleClicked(NodeId const nodeId);

    void nodeHovered(NodeId const nodeId, QPoint const screenPos);

    void nodeHoverLeft(NodeId const nodeId);

    void connectionHovered(ConnectionId const connectionId, QPoint const screenPos);

    void connectionHoverLeft(ConnectionId const connectionId);

    /// Signal allows showing custom context menu upon clicking a node.
    void nodeContextMenu(NodeId const nodeId, QPointF const pos);

private:
    /// @brief Creates Node and Connection graphics objects.
    /**
   * Function is used to populate an empty scene in the constructor. We
   * perform depth-first AbstractGraphModel traversal. The connections are
   * created by checking non-empty node `Out` ports.
   */
    void traverseGraphAndPopulateGraphicsObjects();

    /// Redraws adjacent nodes for given `connectionId`
    void updateAttachedNodes(ConnectionId const connectionId, PortType const portType);

public Q_SLOTS:
    /// Slot called when the `connectionId` is erased form the AbstractGraphModel.
    void onConnectionDeleted(ConnectionId const connectionId);

    /// Slot called when the `connectionId` is created in the AbstractGraphModel.
    void onConnectionCreated(ConnectionId const connectionId);

    void onNodeDeleted(NodeId const nodeId);

    void onNodeCreated(NodeId const nodeId);

    void onNodePositionUpdated(NodeId const nodeId);

    void onNodeUpdated(NodeId const nodeId);

    void onNodeClicked(NodeId const nodeId);

    void onModelReset();

private:
    AbstractGraphModel &_graphModel;

    using UniqueNodeGraphicsObject = std::unique_ptr<NodeGraphicsObject>;

    using UniqueConnectionGraphicsObject = std::unique_ptr<ConnectionGraphicsObject>;

    std::unordered_map<NodeId, UniqueNodeGraphicsObject> _nodeGraphicsObjects;

    std::unordered_map<ConnectionId, UniqueConnectionGraphicsObject> _connectionGraphicsObjects;

    std::unique_ptr<ConnectionGraphicsObject> _draftConnection;

    std::unique_ptr<AbstractNodeGeometry> _nodeGeometry;

    std::unique_ptr<AbstractNodePainter> _nodePainter;

    std::unique_ptr<AbstractConnectionPainter> _connectionPainter;

    bool _nodeDrag;

    QUndoStack *_undoStack;

    Qt::Orientation _orientation;
};

} // namespace QtNodes
