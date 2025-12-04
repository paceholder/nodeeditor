#pragma once

#include <QtCore/QAbstractListModel>
#include <vector>
#include "QtNodes/internal/Definitions.hpp"
#include "QtNodes/internal/Export.hpp"

namespace QtNodes {

class AbstractGraphModel;

class NODE_EDITOR_PUBLIC ConnectionsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        ConnectionIdRole = Qt::UserRole + 1,
        SourceNodeIdRole,
        SourcePortIndexRole,
        DestNodeIdRole,
        DestPortIndexRole
    };

    explicit ConnectionsListModel(std::shared_ptr<AbstractGraphModel> graphModel, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void onConnectionCreated(ConnectionId connectionId);
    void onConnectionDeleted(ConnectionId connectionId);

private:
    std::shared_ptr<AbstractGraphModel> _graphModel;
    std::vector<ConnectionId> _connections;
};

} // namespace QtNodes
