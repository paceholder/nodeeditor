#include "DataFlowGraphicsScene.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "GraphicsView.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "NodeGraphicsObject.hpp"
#include "UndoCommands.hpp"

#include <QGridLayout>
#include <QPushButton>
#include <QStandardItem>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidgetAction>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtGlobal>

#include <stdexcept>
#include <utility>

namespace QtNodes {

DataFlowGraphicsScene::DataFlowGraphicsScene(DataFlowGraphModel &graphModel, QObject *parent)
    : BasicGraphicsScene(graphModel, parent)
    , _graphModel(graphModel)
    , _sortMenu(true)
{
    connect(&_graphModel,
            &DataFlowGraphModel::inPortDataWasSet,
            [this](NodeId const nodeId, PortType const, PortIndex const) { onNodeUpdated(nodeId); });
}

// TODO constructor for an empyt scene?

std::vector<NodeId> DataFlowGraphicsScene::selectedNodes() const
{
    QList<QGraphicsItem *> graphicsItems = selectedItems();

    std::vector<NodeId> result;
    result.reserve(graphicsItems.size());

    for (QGraphicsItem *item : graphicsItems) {
        auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item);

        if (ngo != nullptr) {
            result.push_back(ngo->nodeId());
        }
    }

    return result;
}

QMenu *DataFlowGraphicsScene::createSceneMenu(QPointF const scenePos)
{
    QMenu *modelMenu = new QMenu();

    QWidget *menuWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(menuWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    // Add filterbox to the context menu
    QLineEdit *txtBox = new QLineEdit();
    txtBox->setPlaceholderText(QStringLiteral("Filter"));
    txtBox->setClearButtonEnabled(true);
    txtBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(txtBox, 0, 0);

    // Add sort button
    QPushButton *sortButton = new QPushButton("Sort");
    sortButton->setCheckable(true);
    layout->addWidget(sortButton, 0, 1);

    // Add result treeview to the context menu
    QTreeView *treeView = new QTreeView();
    treeView->header()->close();
    layout->addWidget(treeView, 1, 0, 1, 2);
    QStandardItemModel *treeModel = new QStandardItemModel(treeView);

    auto *menuWidgetAction = new QWidgetAction(menuWidget);
    menuWidgetAction->setDefaultWidget(menuWidget);

    modelMenu->addAction(menuWidgetAction);

    auto registry = _graphModel.dataModelRegistry();

    int sortCount = 0;
    for (auto const &assoc : registry->registeredModelsCategoryAssociation()) {
        QList<QStandardItem *> parentList;
        parentList.clear();
        parentList = treeModel->findItems(assoc.second, Qt::MatchExactly);

        if (parentList.count() <= 0) {
            // Create a parent if it does not exist
            auto parentItem = new QStandardItem(assoc.second);
            parentItem->setData(sortCount, Qt::UserRole);
            parentItem->setFlags(parentItem->flags() & ~Qt::ItemIsSelectable);
            parentList.push_back(parentItem);
            treeModel->appendRow(parentItem);
        }

        auto childItem = new QStandardItem(assoc.first);
        childItem->setData(sortCount, Qt::UserRole);
        parentList.first()->appendRow(childItem);

        sortCount++;
    }

    treeView->setModel(treeModel);
    treeView->expandAll();

    if (_sortMenu) {
        sortButton->setChecked(_sortMenu);
        treeView->sortByColumn(0, Qt::AscendingOrder);
    }

    connect(sortButton, &QPushButton::clicked, this, [this, treeModel, treeView](bool checked) {
        if (checked)
            treeModel->setSortRole(Qt::DisplayRole);
        else
            treeModel->setSortRole(Qt::UserRole);

        _sortMenu = checked;
        treeView->sortByColumn(0, Qt::AscendingOrder);
    });

    connect(treeView,
            &QTreeView::clicked,
            [this, modelMenu, treeModel, scenePos](const QModelIndex &index) {
                auto item = treeModel->itemFromIndex(index);

                if (item->hasChildren())
                    return;

                this->undoStack().push(new CreateCommand(this, item->text(), scenePos));

                modelMenu->close();
            });

    //Setup filtering
    connect(txtBox, &QLineEdit::textChanged, [treeView, treeModel](const QString &text) {
        QModelIndex treeViewIndex = treeView->rootIndex();

        for (int i = 0; i < treeModel->rowCount(); ++i) {
            QStandardItem *parent = treeModel->item(i);
            treeView->setRowHidden(i, treeViewIndex, true);

            for (int j = 0; j < parent->rowCount(); ++j) {
                const bool match = parent->child(j)->text().contains(text, Qt::CaseInsensitive);
                treeView->setRowHidden(j, parent->index(), !match);
                if (match) {
                    treeView->setRowHidden(i, treeViewIndex, false);
                }
            }
        }
    });

    // make sure the text box gets focus so the user doesn't have to click on it
    txtBox->setFocus();

    // QMenu's instance auto-destruction
    modelMenu->setAttribute(Qt::WA_DeleteOnClose);

    return modelMenu;
}

void DataFlowGraphicsScene::sortSceneMenu(bool sortMenu)
{
    _sortMenu = sortMenu;
}

void DataFlowGraphicsScene::save() const
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Open Flow Scene"),
                                                    QDir::homePath(),
                                                    tr("Flow Scene Files (*.flow)"));

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith("flow", Qt::CaseInsensitive))
            fileName += ".flow";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(_graphModel.save()).toJson());
        }
    }
}

void DataFlowGraphicsScene::load()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Open Flow Scene"),
                                                    QDir::homePath(),
                                                    tr("Flow Scene Files (*.flow)"));

    if (!QFileInfo::exists(fileName))
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    clearScene();

    QByteArray const wholeFile = file.readAll();

    _graphModel.load(QJsonDocument::fromJson(wholeFile).object());

    Q_EMIT sceneLoaded();
}

} // namespace QtNodes
