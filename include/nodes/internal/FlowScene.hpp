#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <functional>

#include "QUuidStdHash.hpp"
#include "Export.hpp"
#include "DataModelRegistry.hpp"
#include "TypeConverter.hpp"
#include "memory.hpp"

#include "NodeGroup.hpp"

namespace QtNodes
{

class NodeDataModel;
class FlowItemInterface;
class Node;
class NodeGraphicsObject;
class Connection;
class ConnectionGraphicsObject;
class NodeStyle;
class NodeGroup;
class GroupGraphicsObject;

/**
 * @brief The FlowScene class is responsible for handling nodes and
 * connections. It represents the 2D canvas onto which the graphical
 * elements are drawn, and it controls both the logical models and the
 * graphical objects of NodeEditor.
 */
class NODE_EDITOR_PUBLIC FlowScene
  : public QGraphicsScene
{
  Q_OBJECT
public:

  FlowScene(std::shared_ptr<DataModelRegistry> registry,
            QObject * parent = Q_NULLPTR);

  FlowScene(QObject * parent = Q_NULLPTR);

  ~FlowScene() override;

public:

  std::shared_ptr<Connection>
  createConnection(PortType connectedPort,
                   Node& node,
                   PortIndex portIndex);

  std::shared_ptr<Connection>
  createConnection(Node& nodeIn,
                   PortIndex portIndexIn,
                   Node& nodeOut,
                   PortIndex portIndexOut,
                   TypeConverter const & converter = TypeConverter{});

  std::shared_ptr<Connection> restoreConnection(QJsonObject const &connectionJson);

  /**
   * @brief Loads a connection between nodes from a JSON file into the given map of
   * connections.
   * @param connectionJson JSON object that stores the connection's endpoints.
   * @param nodesMap Map of nodes (i.e. all possible endpoints).
   * @param connectionsMap Map into which the new connection will be added.
   * @param IDMap Map of old node IDs to new node IDs -- to be used when the connections
   * are being restored from a group file to avoid duplicate IDs, since the stored IDs
   * might already be in use.
   * @return Pointer to the newly created connection.
   */
  std::shared_ptr<Connection> loadConnectionToMap(QJsonObject const &connectionJson,
      const std::unordered_map<QUuid, std::unique_ptr<Node>>& nodesMap,
      std::unordered_map<QUuid, std::shared_ptr<Connection>>& connectionsMap,
      const std::unordered_map<QUuid, QUuid>& IDMap = std::unordered_map<QUuid, QUuid>());

  void deleteConnection(Connection& connection);

  TypeConverter getConverter(const QJsonObject& connectionJson);

  Node&createNode(std::unique_ptr<NodeDataModel> && dataModel);

  Node&restoreNode(QJsonObject const& nodeJson);

  Node&loadNodeToMap(QJsonObject const& nodeJson,
                     std::unordered_map<QUuid, std::unique_ptr<Node>>& map,
                     bool restore = false);

  void removeNode(Node& node);

  /**
   * @brief Creates a group in the scene containing the given nodes.
   * @param nodes Reference to the list of nodes to be included in the group.
   * @param name Group's name.
   * @return Pointer to the newly-created group.
   */
  std::weak_ptr<NodeGroup> createGroup(std::vector<Node*>& nodes,
                                       const QUuid& uid = QUuid::createUuid(),
                                       QString name = QStringLiteral(""));

  /**
   * @brief Creates a list of the connections that are incident only to nodes within a
   * given group.
   * @param groupID ID of the desired group.
   * @return List of (pointers of) connections whose both endpoints belong to members of
   * the specified group.
   */
  std::vector<std::shared_ptr<Connection>> connectionsWithinGroup(const QUuid& groupID);

  /**
   * @brief Restores a group from a JSON object.
   * @param groupJson JSON object containing the group data.
   * @return Pointer to the newly-created group.
   */
  std::weak_ptr<NodeGroup> restoreGroup(QJsonObject const& groupJson);

  /**
   * @brief Deletes an empty group. Does nothing if the group isn't empty.
   * @param group Group to be deleted.
   */
  void removeGroup(const QUuid& groupID);

  /**
   * @brief Adds a node to a group.
   * @param nodeID ID of the node that will be included in the group.
   * @param groupID ID of the group that will include the node.
   */
  void addNodeToGroup(const QUuid& nodeID, const QUuid& groupID);

  /**
   * @brief Removes the given node from its current group. The node is not deleted.
   * @param nodeID ID of the node to be removed from its group.
   */
  void removeNodeFromGroup(const QUuid& nodeID);

  DataModelRegistry&registry() const;

  void setRegistry(std::shared_ptr<DataModelRegistry> registry);

  void iterateOverNodes(std::function<void(Node*)> const & visitor);

  void iterateOverNodeData(std::function<void(NodeDataModel*)> const & visitor);

  void iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> const & visitor);

  QPointF getNodePosition(Node const& node) const;

  void setNodePosition(Node& node, QPointF const& pos) const;

  QSizeF getNodeSize(Node const& node) const;

public:

  std::unordered_map<QUuid, std::unique_ptr<Node> > const & nodes() const;

  std::unordered_map<QUuid, std::shared_ptr<Connection> > const & connections() const;

  /**
   * @brief Returns a const reference to the mapping of existing groups.
   */
  std::unordered_map<QUuid, std::shared_ptr<NodeGroup> > const & groups() const;

  std::vector<Node*> allNodes() const;

  std::vector<Node*> selectedNodes() const;

public:

  void clearScene();

  void save() const;

  void load();

  QByteArray saveToMemory() const;

  void loadFromMemory(const QByteArray& data);

  /**
   * @brief Saves a group in a file specified by the user.
   * @param groupID ID of the group to be saved.
   */
  void saveGroupFile(const QUuid& groupID);

  /**
   * @brief Loads a group from a file specified by the user.
   * @return Pointer to the newly-created group.
   */
  std::weak_ptr<NodeGroup> loadGroupFile();

public Q_SLOTS:
 /**
  * @brief Slot called when the quantity of inputs or outputs of a node
  * changes.
  * @param nodeId ID of the altered node
  * @param type Type of port that was changed (input/output)
  * @param nPorts New number of ports of the selected type
  */
 void nodePortsChanged(const QUuid& nodeId,
                       const QtNodes::PortType portType,
                       unsigned int nPorts);

 /**
  * @brief Slot called to insert an input/output port at a specific
  * index of the desired node.
  * @param nodeId ID of the altered node
  * @param portType Type of port (input/output)
  * @param index Index at which the new port will be placed
  */
 void insertNodePort(const QUuid& nodeId,
                     const QtNodes::PortType portType,
                     size_t index);

 /**
  * @brief Slot called to erase an input/output port at a specific
  * index of the desired node.
  * @param nodeId ID of the altered node
  * @param portType Type of port (input/output)
  * @param index Index at which the port will be removed
  */
 void eraseNodePort(const QUuid& nodeId,
                     const QtNodes::PortType portType,
                     size_t index);

Q_SIGNALS:

  /**
   * @brief Node has been created but not on the scene yet.
   * @see nodePlaced()
   */
  void nodeCreated(Node &n);

  /**
   * @brief Node has been added to the scene.
   * @details Connect to this signal if need a correct position of node.
   * @see nodeCreated()
   */
  void nodePlaced(Node &n);

  void nodeDeleted(Node &n);

  void nodeRestored(Node &n);

  void connectionCreated(Connection const &c);
  void connectionDeleted(Connection const &c);

  void nodeMoved(Node& n, const QPointF& newLocation);

  void nodeDoubleClicked(Node& n);

  void nodeClicked(Node& n);

  void connectionHovered(Connection& c, QPoint screenPos);

  void nodeHovered(Node& n, QPoint screenPos);

  void connectionHoverLeft(Connection& c);

  void nodeHoverLeft(Node& n);

  void nodeContextMenu(Node& n, const QPointF& pos);

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;
  using SharedGroup      = std::shared_ptr<NodeGroup>;

  std::unordered_map<QUuid, SharedConnection> _connections{};
  std::unordered_map<QUuid, UniqueNode>       _nodes{};
  std::unordered_map<QUuid, SharedGroup>      _groups{};
  std::shared_ptr<DataModelRegistry>          _registry{};

private Q_SLOTS:

  void setupConnectionSignals(Connection const& c);

  void sendConnectionCreatedToNodes(Connection const& c);
  void sendConnectionDeletedToNodes(Connection const& c);

};

Node*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform const & viewTransform);
}
