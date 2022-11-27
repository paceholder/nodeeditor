#include "DataFlowGraphModel.hpp"

namespace QtNodes
{


DataFlowGraphModel::
DataFlowGraphModel(std::shared_ptr<DataModelRegistry> registry)
  : _registry(registry)
  , _nextNodeId{0}
{}


std::unordered_set<NodeId>
DataFlowGraphModel::
allNodeIds() const
{
  std::unordered_set<NodeId> nodeIds;
  for_each(_models.begin(), _models.end(),
           [&nodeIds](auto const &p)
           {
             nodeIds.insert(p.first);

           });

  return nodeIds;
}


std::unordered_set<std::pair<NodeId, PortIndex>>
DataFlowGraphModel::
connectedNodes(NodeId    nodeId,
               PortType  portType,
               PortIndex portIndex) const
{
  std::unordered_set<std::pair<NodeId, PortIndex>> result;

  auto const key = std::make_tuple(nodeId, portType, portIndex);

  auto it = _connectivity.find(key);

  if (it != _connectivity.end())
    result = it->second;

  return result;
}


NodeId
DataFlowGraphModel::
addNode(QString const nodeType)
{
  std::unique_ptr<NodeDataModel> model =
    _registry->create(nodeType);

  if (model)
  {
    NodeId newId = newNodeId();

    connect(model.get(), &NodeDataModel::dataUpdated,
            [newId, this](PortIndex const portIndex)
            { onNodeDataUpdated(newId, portIndex); });

    _models[newId] = std::move(model);

    Q_EMIT nodeCreated(newId);

    return newId;
  }

  return InvalidNodeId;
}


void
DataFlowGraphModel::
addConnection(ConnectionId const connectionId)
{
  auto connect =
    [&](PortType portType)
    {
      auto key = std::make_tuple(getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId));

      PortType opposite = oppositePort(portType);

      _connectivity[key].insert(std::make_pair(getNodeId(opposite, connectionId),
                                               getPortIndex(opposite, connectionId)));
    };

  connect(PortType::Out);
  connect(PortType::In);

  Q_EMIT connectionCreated(connectionId);

  onNodeDataUpdated(getNodeId(PortType::Out, connectionId),
                    getPortIndex(PortType::Out, connectionId));
}


QVariant
DataFlowGraphModel::
nodeData(NodeId nodeId, NodeRole role) const
{
  QVariant result;

  auto it = _models.find(nodeId);
  if (it == _models.end())
    return result;

  auto &model = it->second;

  switch (role)
  {
    case NodeRole::Type:
      result = model->name();
      break;

    case NodeRole::Position:
      result = _nodeGeometryData[nodeId].pos;
      break;

    case NodeRole::Size:
      result = _nodeGeometryData[nodeId].size;
      break;

    case NodeRole::CaptionVisible:
      result = model->captionVisible();
      break;

    case NodeRole::Caption:
      result = model->caption();
      break;

    case NodeRole::Style:
      {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariant();
      }
      break;

    case NodeRole::NumberOfInPorts:
      result = model->nPorts(PortType::In);
      break;

    case NodeRole::NumberOfOutPorts:
      result = model->nPorts(PortType::Out);
      break;

    case NodeRole::Widget:
      {
        auto w = model->embeddedWidget();
        result = QVariant::fromValue(w);
      }
      break;
  }

  return result;
}


NodeFlags
DataFlowGraphModel::
nodeFlags(NodeId nodeId) const
{
  auto it = _models.find(nodeId);

  if (it != _models.end() && it->second->resizable())
    return NodeFlag::Resizable;

  return GraphModel::nodeFlags(nodeId);
}


bool
DataFlowGraphModel::
setNodeData(NodeId   nodeId,
            NodeRole role,
            QVariant value)
{
  Q_UNUSED(nodeId);
  Q_UNUSED(role);
  Q_UNUSED(value);

  bool result = false;

  switch (role)
  {
    case NodeRole::Type:
      break;
    case NodeRole::Position:
      {
        _nodeGeometryData[nodeId].pos = value.value<QPointF>();

        Q_EMIT nodePositonUpdated(nodeId);

        result = true;
      }
      break;

    case NodeRole::Size:
      {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result = true;
      }
      break;

    case NodeRole::CaptionVisible:
      break;

    case NodeRole::Caption:
      break;

    case NodeRole::Style:
      break;

    case NodeRole::NumberOfInPorts:
      break;

    case NodeRole::NumberOfOutPorts:
      break;

    case NodeRole::Widget:
      break;
  }

  return result;
}


QVariant
DataFlowGraphModel::
portData(NodeId    nodeId,
         PortType  portType,
         PortIndex portIndex,
         PortRole  role) const
{
  QVariant result;

  auto it = _models.find(nodeId);
  if (it == _models.end())
    return result;

  auto &model = it->second;

  switch (role)
  {
    case PortRole::Data:
      if (portType == PortType::Out)
        result = QVariant::fromValue(model->outData(portIndex));
      break;

    case PortRole::DataType:
      result = QVariant::fromValue(model->dataType(portType, portIndex));
      break;

    case PortRole::ConnectionPolicy:
      if (portType == PortType::Out)
        result = QVariant::fromValue(model->portOutConnectionPolicy(portIndex));
      else
        result = QVariant::fromValue(ConnectionPolicy::One);
      break;

    case PortRole::CaptionVisible:
      result = model->portCaptionVisible(portType, portIndex);
      break;

    case PortRole::Caption:
      result = model->portCaption(portType, portIndex);

      break;
  }

  return result;
}


bool
DataFlowGraphModel::
setPortData(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex,
            PortRole  role) const
{
  Q_UNUSED(nodeId);
  Q_UNUSED(portType);
  Q_UNUSED(portIndex);
  Q_UNUSED(role);

  return false;
}


bool
DataFlowGraphModel::
deleteConnection(ConnectionId const connectionId)
{
  bool disconnected = false;

  auto disconnect =
    [&](PortType portType)
    {
      auto key = std::make_tuple(getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId));
      auto it = _connectivity.find(key);

      if (it != _connectivity.end())
      {
        disconnected = true;

        PortType opposite = oppositePort(portType);

        auto oppositePair =
          std::make_pair(getNodeId(opposite, connectionId),
                         getPortIndex(opposite, connectionId));
        it->second.erase(oppositePair);

        if (it->second.empty())
        {
          _connectivity.erase(it);
        }
      }

    };

  disconnect(PortType::Out);
  disconnect(PortType::In);

  if (disconnected)
  {
    Q_EMIT connectionDeleted(connectionId);

    propagateEmptyDataTo(getNodeId(PortType::In, connectionId),
                         getPortIndex(PortType::In, connectionId));

  }

  return disconnected;
}


bool
DataFlowGraphModel::
deleteNode(NodeId const nodeId)
{
  // Delete connections to this node first.
  auto connectionIds = allConnectionIds(nodeId);
  for (auto &cId : connectionIds)
  {
    deleteConnection(cId);
  }

  _nodeGeometryData.erase(nodeId);
  _models.erase(nodeId);

  Q_EMIT nodeDeleted(nodeId);

  return true;
}


void
DataFlowGraphModel::
onNodeDataUpdated(NodeId const    nodeId,
                  PortIndex const portIndex)
{

  auto const &connected =
    connectedNodes(nodeId, PortType::Out, portIndex);

  // TODO: Should we pull the data through the model?
  /*
     auto outPortData =
     portData(nodeId,
     PortType::Out,
     portIndex,
     PortRole::Data).value<std::shared_ptr<NodeData>>();
   */

  auto const outPortData =
    _models[nodeId]->outData(portIndex);

  for (auto const &cn : connected)
  {
    _models[cn.first]->setInData(outPortData, cn.second);

    Q_EMIT portDataSet(cn.first, PortType::In, cn.second);
  }
}


void

DataFlowGraphModel::
propagateEmptyDataTo(NodeId const    nodeId,
                     PortIndex const portIndex)
{
  auto const emptyData = std::shared_ptr<NodeData>();

  _models[nodeId]->setInData(emptyData, portIndex);

  Q_EMIT portDataSet(nodeId, PortType::In, portIndex);
}


}

