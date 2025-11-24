#pragma once

#include <QtCore/QAbstractListModel>
#include <QtCore/QPointF>
#include <vector>
#include "QtNodes/internal/Definitions.hpp"

namespace QtNodes {

class AbstractGraphModel;

class NodesListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        NodeIdRole = Qt::UserRole + 1,
        TypeRole,
        PositionRole,
        CaptionRole,
        InPortCountRole,
        OutPortCountRole,
        DelegateModelRole,
        ResizableRole,
        WidthRole,
        HeightRole
    };

    explicit NodesListModel(std::shared_ptr<AbstractGraphModel> graphModel, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool moveNode(int nodeId, double x, double y);

public Q_SLOTS:
    void onNodeCreated(NodeId nodeId);
    void onNodeDeleted(NodeId nodeId);
    void onNodePositionUpdated(NodeId nodeId);
    void onNodeUpdated(NodeId nodeId);

private:
    std::shared_ptr<AbstractGraphModel> _graphModel;
    std::vector<NodeId> _nodeIds;
};

} // namespace QtNodes
