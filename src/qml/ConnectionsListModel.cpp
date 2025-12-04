#include "QtNodes/qml/ConnectionsListModel.hpp"

#include "QtNodes/internal/AbstractGraphModel.hpp"

namespace QtNodes {

ConnectionsListModel::ConnectionsListModel(std::shared_ptr<AbstractGraphModel> graphModel, QObject *parent)
    : QAbstractListModel(parent)
    , _graphModel(std::move(graphModel))
{
    if (_graphModel) {
        connect(_graphModel.get(), &AbstractGraphModel::connectionCreated, this, &ConnectionsListModel::onConnectionCreated);
        connect(_graphModel.get(), &AbstractGraphModel::connectionDeleted, this, &ConnectionsListModel::onConnectionDeleted);

        // Initialize with existing connections
        // Since there's no 'allConnectionIds' global accessor, we iterate nodes
        // Wait, AbstractGraphModel doesn't have 'allConnectionIds()' without args?
        // Checking AbstractGraphModel.hpp:
        // virtual std::unordered_set<NodeId> allNodeIds() const = 0;
        // virtual std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const = 0;
        // It does not have a global allConnectionIds.
        
        auto nodeIds = _graphModel->allNodeIds();
        for (auto nodeId : nodeIds) {
             auto connections = _graphModel->allConnectionIds(nodeId);
             for (auto& conn : connections) {
                 // Avoid duplicates. Since connections are directed (Out->In), 
                 // we can just store them all, but 'allConnectionIds(nodeId)' returns 
                 // connections attached to the node (both in and out).
                 // So we will see each connection twice.
                 // We should only add if this node is the output node (or input node).
                 // ConnectionId struct: outNodeId, outPortIndex, inNodeId, inPortIndex.
                 
                 if (conn.outNodeId == nodeId) {
                     _connections.push_back(conn);
                 }
             }
        }
    }
}

int ConnectionsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(_connections.size());
}

QVariant ConnectionsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(_connections.size()))
        return QVariant();

    const auto& conn = _connections[index.row()];

    switch (role) {
    case SourceNodeIdRole:
        return QVariant::fromValue<int>(static_cast<int>(conn.outNodeId));
    case SourcePortIndexRole:
        return QVariant::fromValue<int>(static_cast<int>(conn.outPortIndex));
    case DestNodeIdRole:
        return QVariant::fromValue<int>(static_cast<int>(conn.inNodeId));
    case DestPortIndexRole:
        return QVariant::fromValue<int>(static_cast<int>(conn.inPortIndex));
    }

    return QVariant();
}

QHash<int, QByteArray> ConnectionsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SourceNodeIdRole] = "sourceNodeId";
    roles[SourcePortIndexRole] = "sourcePortIndex";
    roles[DestNodeIdRole] = "destNodeId";
    roles[DestPortIndexRole] = "destPortIndex";
    return roles;
}

void ConnectionsListModel::onConnectionCreated(ConnectionId connectionId)
{
    beginInsertRows(QModelIndex(), static_cast<int>(_connections.size()), static_cast<int>(_connections.size()));
    _connections.push_back(connectionId);
    endInsertRows();
}

void ConnectionsListModel::onConnectionDeleted(ConnectionId connectionId)
{
    auto it = std::find(_connections.begin(), _connections.end(), connectionId);
    if (it != _connections.end()) {
        int index = static_cast<int>(std::distance(_connections.begin(), it));
        beginRemoveRows(QModelIndex(), index, index);
        _connections.erase(it);
        endRemoveRows();
    }
}

} // namespace QtNodes
