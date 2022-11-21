#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <memory>
#include <functional>

#include "QUuidStdHash.hpp"
#include "Export.hpp"
#include "DataModelRegistry.hpp"
#include <stack>

namespace QtNodes
{

class NodeDataModel;
class FlowItemInterface;
class Node;
class Group;
class NodeGraphicsObject;
class Connection;
class ConnectionGraphicsObject;
class NodeStyle;

struct SceneHistory
{
	QByteArray data;
};


struct UndoRedoAction {

  std::function<int(double)> undoAction;
  std::function<int(double)> redoAction;
  UndoRedoAction(std::function<int(double)> undoAction, std::function<int(double)> redoAction) {
    this->undoAction = undoAction;
    this->redoAction = redoAction;
  };
};


struct Anchor {
  QPointF position;
  double scale;
};

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC FlowScene
  : public QGraphicsScene
{
  Q_OBJECT
public:

  FlowScene(std::shared_ptr<DataModelRegistry> registry =
              std::make_shared<DataModelRegistry>());

  ~FlowScene();

  std::stack<UndoRedoAction> actionsHistory;
public:

  std::shared_ptr<Connection>createConnection(PortType connectedPort,
                                              Node& node,
                                              PortIndex portIndex);

  std::shared_ptr<Connection>createConnection(Node& nodeIn,
                                              PortIndex portIndexIn,
                                              Node& nodeOut,
                                              PortIndex portIndexOut);

  std::shared_ptr<Connection>restoreConnection(QJsonObject const &connectionJson);

  void deleteConnection(Connection& connection);
  void deleteConnection(Connection* connection);

  Node&createNode(std::unique_ptr<NodeDataModel> && dataModel);

  Group& createGroup();
  
  Group& pasteGroup(QJsonObject const& nodeJson, QPointF nodeGroupCentroid, QPointF mousePos);

  Node&restoreNode(QJsonObject const& nodeJson);
  
  Group& restoreGroup(QJsonObject const& nodeJson);

  QUuid pasteNode(QJsonObject &json, QPointF nodeGroupCentroid, QPointF mousePos);
  
  void pasteConnection(QJsonObject const &connectionJson, QUuid newIn, QUuid newOut);

  void removeNode(Node& node);

  void removeGroup(Group& node);

  DataModelRegistry&registry() const;

  void setRegistry(std::shared_ptr<DataModelRegistry> registry);

  void iterateOverNodes(std::function<void(Node*)> visitor);

  void iterateOverNodeData(std::function<void(NodeDataModel*)> visitor);

  void iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> visitor);

  QPointF getNodePosition(const Node& node) const;

  void setNodePosition(Node& node, const QPointF& pos) const;

  QSizeF getNodeSize(const Node& node) const;

  void resolveGroups(Group& node);

  void resolveGroups(Node& n);

public:

  std::unordered_map<QUuid, std::shared_ptr<Node> > const &nodes() const;

  std::unordered_map<QUuid, std::shared_ptr<Connection> > const &connections() const;

  std::vector<std::shared_ptr<Node>>selectedNodes() const;

public:

  void clearScene();

  void save() const;

  void load();

  QByteArray saveToMemory() const;

  void saveToClipBoard();

  void loadFromMemory(const QByteArray& data);
  
  void Undo();
  
  void Redo();
  
  void UpdateHistory();
  
  void ResetHistory();

  int GetHistoryIndex();

signals:

  void nodeCreated(Node &n);
  
  void groupCreated(Group &g);

  void nodeDeleted(Node &n);

  void connectionCreated(Connection &c);

  void connectionDeleted(Connection &c);

  void nodeMoved(Node& n, const QPointF& newLocation);

  void groupMoved(Group& n, const QPointF& newLocation);
  
  void nodeMoveFinished(Node& n, const QPointF& newLocation);
  
  void groupMoveFinished(Group& g, const QPointF& newLocation);

  void nodeDoubleClicked(Node& n);
  
  void groupDoubleClicked(Group& g);

  void connectionHovered(Connection& c, QPoint screenPos);

  void nodeHovered(Node& n, QPoint screenPos);

  void connectionHoverLeft(Connection& c);

  void nodeHoverLeft(Node& n);

  void nodeContextMenu(Node& n, const QPointF& pos);

public:

  std::vector<Anchor> anchors;  

  int gridSize=1;
  bool snapping=false;

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::shared_ptr<Node>;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, UniqueNode>       _nodes;
  std::shared_ptr<DataModelRegistry>          _registry;
  std::unordered_map<QUuid, std::shared_ptr<Group>> _groups;

  int historyInx; 
  bool writeToHistory; 
  std::vector< SceneHistory > history;


private: 
};

Node*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform viewTransform);
Group*
locateGroupAt(QPointF scenePoint, FlowScene &scene,
             QTransform viewTransform);
}
