#pragma once

#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "ConnectionIdHash.hpp"
#include "Definitions.hpp"
#include "Export.hpp"
#include "GroupGraphicsObject.hpp"
#include "NodeGroup.hpp"
#include "UndoCommands.hpp"

#include <QtCore/QJsonObject>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

class QUndoStack;

namespace QtNodes {

class AbstractConnectionPainter;
class AbstractGraphModel;
class AbstractNodePainter;
class ConnectionGraphicsObject;
class NodeGraphicsObject;
class NodeStyle;
class DeleteCommand;
class CopyCommand;
class NodeGroup;
class GroupGraphicsObject;
struct ConnectionId;

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

    /**
     * @brief Setter for the _groupingEnabled flag.
     * @param boolean to set or not the flag.
     */
    void setGroupingEnabled(bool enabled);

    /**
     * @brief Getter for the _groupingEnabled flag.
     */
    bool groupingEnabled() const { return _groupingEnabled; }

public:
    /**
     * @brief Creates a "draft" instance of ConnectionGraphicsObject.
     *
     * The scene caches a "draft" connection which has one loose end.
     * After attachment the "draft" instance is deleted and instead a
     * normal "full" connection is created.
     * Function @returns the "draft" instance for further geometry
     * manipulations.
     */
    std::unique_ptr<ConnectionGraphicsObject> const &makeDraftConnection(
        ConnectionId const newConnectionId);

    /**
     * @brief Deletes "draft" connection.
     *
     * The function is called when user releases the mouse button during
     * the construction of the new connection without attaching it to any
     * node.
     */
    void resetDraftConnection();

    /// Deletes all the nodes. Connections are removed automatically.
    void clearScene();

    /**
     * @brief Creates a list of the connections that are incident only to nodes within a
     * given group.
     * @param groupID ID of the desired group.
     * @return List of (pointers of) connections whose both endpoints belong to members of
     * the specified group.
     */
    std::vector<std::shared_ptr<ConnectionId>> connectionsWithinGroup(GroupId groupID);
    /**
     * @brief Creates a group in the scene containing the given nodes.
     * @param nodes Reference to the list of nodes to be included in the group.
     * @param name Group's name.
     * @param groupId Group's id.
     * @return Pointer to the newly-created group.
     */
    std::weak_ptr<NodeGroup> createGroup(std::vector<NodeGraphicsObject *> &nodes,
                                         QString name = QStringLiteral(""),
                                         GroupId groupId = InvalidGroupId);

    /**
     * @brief Creates a group in the scene containing the currently selected nodes.
     * @param name Group's name
     * @return Pointer to the newly-created group.
     */
    std::weak_ptr<NodeGroup> createGroupFromSelection(QString groupName = QStringLiteral(""));

    /**
     * @brief Restores a group from a JSON object.
     * @param groupJson JSON object containing the group data.
     * @return Pair consisting of a pointer to the newly-created group and the mapping
     * between old and new nodes.
     */
    std::pair<std::weak_ptr<NodeGroup>, std::unordered_map<GroupId, GroupId>> restoreGroup(
        QJsonObject const &groupJson);

    /**
     * @brief Returns a const reference to the mapping of existing groups.
     */
    std::unordered_map<GroupId, std::shared_ptr<NodeGroup>> const &groups() const;

    /**
     * @brief Loads a group from a file specified by the user.
     * @return Pointer to the newly-created group.
     */
    std::weak_ptr<NodeGroup> loadGroupFile();

    /**
     * @brief Saves a group in a .group file.
     * @param groupID Group's id.
     */
    void saveGroupFile(GroupId groupID);

    /**
     * @brief Calculates the selected nodes.
     * @return Vector containing the NodeGraphicsObject pointers related to the selected nodes.
     */
    std::vector<NodeGraphicsObject *> selectedNodes() const;

    /**
     * @brief Calculates the selected groups.
     * @return Vector containing the GroupGraphicsObject pointers related to the selected groups.
     */
    std::vector<GroupGraphicsObject *> selectedGroups() const;

    /**
     * @brief Adds a node to a group, if both node and group exists.
     * @param nodeId Node's id.
     * @param groupId Group's id.
     */
    void addNodeToGroup(NodeId nodeId, GroupId groupId);

    /**
     * @brief Removes a node from a group, if the node exists and is within a group.
     * @param nodeId Node's id.
     */
    void removeNodeFromGroup(NodeId nodeId);

public:
    /**
     * @returns NodeGraphicsObject associated with the given nodeId.
     * @returns nullptr when the object is not found.
     */
    NodeGraphicsObject *nodeGraphicsObject(NodeId nodeId);

    /**
     * @returns ConnectionGraphicsObject corresponding to `connectionId`.
     * @returns `nullptr` when the object is not found.
     */
    ConnectionGraphicsObject *connectionGraphicsObject(ConnectionId connectionId);

    Qt::Orientation orientation() const { return _orientation; }

    void setOrientation(Qt::Orientation const orientation);

public:
    /**
     * Can @return an instance of the scene context menu in subclass.
     * Default implementation returns `nullptr`.
     */
    virtual QMenu *createSceneMenu(QPointF const scenePos);

    /**
     * @brief Creates the default menu when a node is selected.
     */
    QMenu *createStdMenu(QPointF const scenePos);

    /**
     * @brief Creates the menu when a group is selected.
     * @param groupGo reference to the GroupGraphicsObject related to the selected group.
     */
    QMenu *createGroupMenu(QPointF const scenePos, GroupGraphicsObject *groupGo);

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
    /**
     * @brief Creates Node and Connection graphics objects.
     * 
     * Function is used to populate an empty scene in the constructor. We
     * perform depth-first AbstractGraphModel traversal. The connections are
     * created by checking non-empty node `Out` ports.
     */
    void traverseGraphAndPopulateGraphicsObjects();

    /// Redraws adjacent nodes for given `connectionId`
    void updateAttachedNodes(ConnectionId const connectionId, PortType const portType);

    /**
     * @brief Loads a JSON object that represents a node, with the option
     * to keep the stored node id or generate a new one.
     * @param nodeJson The JSON object representing a node.
     * @param keepOriginalId If true, the loaded node will have the same id as the one stored in
     * the file; otherwise, a new id will be generated
     * @return A reference to the NodeGraphicsObject related to the loaded node.
     */
    NodeGraphicsObject &loadNodeToMap(QJsonObject nodeJson, bool keepOriginalId = false);

    /**
     * @brief Loads a connection between nodes from a JSON file.
     * @param connectionJson JSON object that stores the connection's endpoints.
     * @param nodeIdMap Map of nodes (i.e. all possible endpoints).
     */
    void loadConnectionToMap(QJsonObject const &connectionJson,
                             std::unordered_map<NodeId, NodeId> const &nodeIdMap);

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

    /**
     * @brief Slot called to trigger the copy command action.
     */
    void onCopySelectedObjects() { undoStack().push(new CopyCommand(this)); }

    /**
     * @brief Slot called to trigger the delete command action.
     */
    void onDeleteSelectedObjects() { undoStack().push(new DeleteCommand(this)); }

private:
    AbstractGraphModel &_graphModel;

    using UniqueNodeGraphicsObject = std::unique_ptr<NodeGraphicsObject>;
    using UniqueConnectionGraphicsObject = std::unique_ptr<ConnectionGraphicsObject>;
    using SharedGroup = std::shared_ptr<NodeGroup>;

    std::unordered_map<NodeId, UniqueNodeGraphicsObject> _nodeGraphicsObjects;
    std::unordered_map<ConnectionId, UniqueConnectionGraphicsObject> _connectionGraphicsObjects;
    GroupId nextGroupId();

    std::unordered_map<GroupId, SharedGroup> _groups{};
    GroupId _nextGroupId{0};
    std::unique_ptr<ConnectionGraphicsObject> _draftConnection;
    std::unique_ptr<AbstractNodeGeometry> _nodeGeometry;
    std::unique_ptr<AbstractNodePainter> _nodePainter;
    std::unique_ptr<AbstractConnectionPainter> _connectionPainter;
    bool _nodeDrag;
    QUndoStack *_undoStack;
    Qt::Orientation _orientation;
    bool _groupingEnabled;
};

} // namespace QtNodes
