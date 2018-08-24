#include "FlowSceneModel.hpp"
#include "DataModelRegistry.hpp"
#include "ConnectionID.hpp"
#include "QUuidStdHash.hpp"
#include "Export.hpp"

namespace QtNodes {

class Node;
class Connection;

/// Model for simple data flow
class NODE_EDITOR_PUBLIC DataFlowModel 
  : public FlowSceneModel
{
public:

  DataFlowModel(std::shared_ptr<DataModelRegistry> reg);

  // FlowSceneModel read interface
  QStringList
  modelRegistry() const override;

  QString
  nodeTypeCatergory(QString const& name) const override;

  bool
  getTypeConvertable(TypeConverterId const& id ) const override;

  QList<QUuid>
  nodeUUids() const override;

  NodeIndex
  nodeIndex(const QUuid& ID) const override;

  QString
  nodeTypeIdentifier(NodeIndex const& index) const override;

  QString
  nodeCaption(NodeIndex const& index) const override;

  QPointF
  nodeLocation(NodeIndex const& index) const override;

  QWidget*
  nodeWidget(NodeIndex const& index) const override;

  bool
  nodeResizable(NodeIndex const& index) const override;

  NodeValidationState
  nodeValidationState(NodeIndex const& index) const override;

  QString
  nodeValidationMessage(NodeIndex const& index) const override;

  NodePainterDelegate*
  nodePainterDelegate(NodeIndex const& index) const override;

  unsigned int
  nodePortCount(NodeIndex const& index, PortType portType) const override;

  QString
  nodePortCaption(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  NodeDataType
  nodePortDataType(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  ConnectionPolicy
  nodePortConnectionPolicy(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  std::vector<std::pair<NodeIndex, PortIndex> >
  nodePortConnections(NodeIndex const& index, PortIndex id, PortType portType) const override;

  // FlowSceneModel write interface
  bool
  removeConnection(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID) override;
  bool
  addConnection(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID) override;
  bool
  removeNode(NodeIndex const& index) override;
  QUuid
  addNode(QString const& typeID, QPointF const& location) override;
  bool
  moveNode(NodeIndex const& index, QPointF newLocation) override;

  // convenience functions
  QUuid
  addNode(QString const& typeID, QPointF const& location, QUuid const& uuid);
  QUuid
  addNode(std::unique_ptr<NodeDataModel>&& model, QPointF const& location, QUuid const& uuid = QUuid::createUuid());
  ConnectionID
  addConnection(Node* left, PortIndex leftIdx, Node* right, PortIndex rightIdx, TypeConverter converter);

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  std::unordered_map<ConnectionID, SharedConnection>& connections() { return _connections; }
  std::unordered_map<QUuid, UniqueNode>& nodes() { return _nodes; }
  DataModelRegistry& registry() { return *_registry; }
  void setRegistry(std::shared_ptr<DataModelRegistry> registry) {
    _registry = std::move(registry);
  }

private:

  std::unordered_map<QUuid, UniqueNode>              _nodes;
  std::unordered_map<ConnectionID, SharedConnection> _connections;
  std::shared_ptr<DataModelRegistry>                 _registry;
};

}