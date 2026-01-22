#include "QtNodes/qml/NodesListModel.hpp"

#include "QtNodes/internal/AbstractGraphModel.hpp"
#include "QtNodes/internal/DataFlowGraphModel.hpp"
#include "QtNodes/internal/NodeDelegateModel.hpp"

namespace QtNodes {

NodesListModel::NodesListModel(std::shared_ptr<AbstractGraphModel> graphModel, QObject *parent)
    : QAbstractListModel(parent)
    , _graphModel(std::move(graphModel))
{
    if (_graphModel) {
        connect(_graphModel.get(), &AbstractGraphModel::nodeCreated, this, &NodesListModel::onNodeCreated);
        connect(_graphModel.get(), &AbstractGraphModel::nodeDeleted, this, &NodesListModel::onNodeDeleted);
        connect(_graphModel.get(), &AbstractGraphModel::nodePositionUpdated, this, &NodesListModel::onNodePositionUpdated);
        connect(_graphModel.get(), &AbstractGraphModel::nodeUpdated, this, &NodesListModel::onNodeUpdated);

        // Initialize with existing nodes
        auto ids = _graphModel->allNodeIds();
        _nodeIds.reserve(ids.size());
        for (auto id : ids) {
            _nodeIds.push_back(id);
        }
    }
}

int NodesListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(_nodeIds.size());
}

QVariant NodesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(_nodeIds.size()))
        return QVariant();

    NodeId nodeId = _nodeIds[index.row()];

    switch (role) {
    case NodeIdRole:
        return QVariant::fromValue<int>(static_cast<int>(nodeId));
    case TypeRole:
        return _graphModel->nodeData(nodeId, NodeRole::Type);
    case PositionRole:
        return _graphModel->nodeData(nodeId, NodeRole::Position);
    case CaptionRole:
        return _graphModel->nodeData(nodeId, NodeRole::Caption);
    case InPortCountRole:
        return _graphModel->nodeData(nodeId, NodeRole::InPortCount);
    case OutPortCountRole:
        return _graphModel->nodeData(nodeId, NodeRole::OutPortCount);
    case ResizableRole:
        return bool(_graphModel->nodeFlags(nodeId) & NodeFlag::Resizable);
    case WidthRole:
        return _graphModel->nodeData(nodeId, NodeRole::Size).toSize().width();
    case HeightRole:
        return _graphModel->nodeData(nodeId, NodeRole::Size).toSize().height();
    case DelegateModelRole: {
        auto dfModel = std::dynamic_pointer_cast<DataFlowGraphModel>(_graphModel);
        if (dfModel) {
            auto model = dfModel->delegateModel<NodeDelegateModel>(nodeId);
            return QVariant::fromValue<QObject*>(model);
        }
        return QVariant();
    }
    }

    return QVariant();
}

QHash<int, QByteArray> NodesListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NodeIdRole] = "nodeId";
    roles[TypeRole] = "nodeType";
    roles[PositionRole] = "position";
    roles[CaptionRole] = "caption";
    roles[InPortCountRole] = "inPorts";
    roles[OutPortCountRole] = "outPorts";
    roles[DelegateModelRole] = "delegateModel";
    roles[ResizableRole] = "resizable";
    roles[WidthRole] = "width";
    roles[HeightRole] = "height";
    return roles;
}

bool NodesListModel::moveNode(int nodeId, double x, double y)
{
    return _graphModel->setNodeData(static_cast<NodeId>(nodeId), NodeRole::Position, QPointF(x, y));
}

void NodesListModel::onNodeCreated(NodeId nodeId)
{
    beginInsertRows(QModelIndex(), static_cast<int>(_nodeIds.size()), static_cast<int>(_nodeIds.size()));
    _nodeIds.push_back(nodeId);
    endInsertRows();
}

void NodesListModel::onNodeDeleted(NodeId nodeId)
{
    auto it = std::find(_nodeIds.begin(), _nodeIds.end(), nodeId);
    if (it != _nodeIds.end()) {
        int index = static_cast<int>(std::distance(_nodeIds.begin(), it));
        beginRemoveRows(QModelIndex(), index, index);
        _nodeIds.erase(it);
        endRemoveRows();
    }
}

void NodesListModel::onNodePositionUpdated(NodeId nodeId)
{
    auto it = std::find(_nodeIds.begin(), _nodeIds.end(), nodeId);
    if (it != _nodeIds.end()) {
        int index = static_cast<int>(std::distance(_nodeIds.begin(), it));
        QModelIndex idx = createIndex(index, 0);
        Q_EMIT dataChanged(idx, idx, {PositionRole});
    }
}

void NodesListModel::onNodeUpdated(NodeId nodeId)
{
    auto it = std::find(_nodeIds.begin(), _nodeIds.end(), nodeId);
    if (it != _nodeIds.end()) {
        int index = static_cast<int>(std::distance(_nodeIds.begin(), it));
        QModelIndex idx = createIndex(index, 0);
        Q_EMIT dataChanged(idx, idx); // Update all roles
    }
}

} // namespace QtNodes
