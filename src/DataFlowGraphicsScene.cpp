#include "DataFlowGraphicsScene.hpp"

#include "CommentGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "GraphicsView.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "NodeGraphicsObject.hpp"
#include "UndoCommands.hpp"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
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

    // Add filterbox to the context menu
    auto *txtBox = new QLineEdit(modelMenu);
    txtBox->setPlaceholderText(QStringLiteral("Filter"));
    txtBox->setClearButtonEnabled(true);

    auto *txtBoxAction = new QWidgetAction(modelMenu);
    txtBoxAction->setDefaultWidget(txtBox);

    // 1.
    modelMenu->addAction(txtBoxAction);

    // Add result treeview to the context menu
    QTreeWidget *treeView = new QTreeWidget(modelMenu);
    treeView->header()->close();

    auto *treeViewAction = new QWidgetAction(modelMenu);
    treeViewAction->setDefaultWidget(treeView);

    // 2.
    modelMenu->addAction(treeViewAction);

    auto registry = _graphModel.dataModelRegistry();

    for (auto const &cat : registry->categories()) {
        auto item = new QTreeWidgetItem(treeView);
        item->setText(0, cat);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    for (auto const &assoc : registry->registeredModelsCategoryAssociation()) {
        QList<QTreeWidgetItem *> parent = treeView->findItems(assoc.second, Qt::MatchExactly);

        if (parent.count() <= 0)
            continue;

        auto item = new QTreeWidgetItem(parent.first());
        item->setText(0, assoc.first);
    }

    treeView->expandAll();

    connect(treeView,
            &QTreeWidget::itemClicked,
            [this, modelMenu, scenePos](QTreeWidgetItem *item, int) {
                if (!(item->flags() & (Qt::ItemIsSelectable))) {
                    return;
                }

                this->undoStack().push(new CreateCommand(this, item->text(0), scenePos));

                modelMenu->close();
            });

    //Setup filtering
    connect(txtBox, &QLineEdit::textChanged, [treeView](const QString &text) {
        QTreeWidgetItemIterator categoryIt(treeView, QTreeWidgetItemIterator::HasChildren);
        while (*categoryIt)
            (*categoryIt++)->setHidden(true);
        QTreeWidgetItemIterator it(treeView, QTreeWidgetItemIterator::NoChildren);
        while (*it) {
            auto modelName = (*it)->text(0);
            const bool match = (modelName.contains(text, Qt::CaseInsensitive));
            (*it)->setHidden(!match);
            if (match) {
                QTreeWidgetItem *parent = (*it)->parent();
                while (parent) {
                    parent->setHidden(false);
                    parent = parent->parent();
                }
            }
            ++it;
        }
    });

    // make sure the text box gets focus so the user doesn't have to click on it
    txtBox->setFocus();

    // QMenu's instance auto-destruction
    modelMenu->setAttribute(Qt::WA_DeleteOnClose);

    return modelMenu;
}

bool DataFlowGraphicsScene::save() const
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
            QJsonObject sceneJson = _graphModel.save();
            
            // Add comments to the saved data
            QJsonArray commentsArray;
            for (auto const& [commentId, commentGO] : comments()) {
                QJsonObject commentJson;
                commentJson["id"] = commentId.toString();
                commentJson["text"] = commentGO->commentText();
                commentJson["pos"] = QJsonObject{{"x", commentGO->pos().x()}, {"y", commentGO->pos().y()}};
                
                QRectF rect = commentGO->boundingRect();
                commentJson["rect"] = QJsonObject{
                    {"x", rect.x()},
                    {"y", rect.y()},
                    {"width", rect.width()},
                    {"height", rect.height()}
                };
                
                // Save grouped node IDs
                QJsonArray groupedNodes;
                for (NodeId nodeId : commentGO->groupedNodes()) {
                    groupedNodes.append(static_cast<qint64>(nodeId));
                }
                commentJson["groupedNodes"] = groupedNodes;
                
                commentsArray.append(commentJson);
            }
            sceneJson["comments"] = commentsArray;
            
            file.write(QJsonDocument(sceneJson).toJson());
            return true;
        }
    }
    return false;
}

bool DataFlowGraphicsScene::load()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Open Flow Scene"),
                                                    QDir::homePath(),
                                                    tr("Flow Scene Files (*.flow)"));

    if (!QFileInfo::exists(fileName))
        return false;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    clearScene();

    QByteArray const wholeFile = file.readAll();
    QJsonObject sceneJson = QJsonDocument::fromJson(wholeFile).object();

    _graphModel.load(sceneJson);

    // Load comments
    QJsonArray commentsArray = sceneJson["comments"].toArray();
    for (QJsonValue commentVal : commentsArray) {
        QJsonObject commentJson = commentVal.toObject();
        
        QUuid commentId = QUuid::fromString(commentJson["id"].toString());
        auto commentGO = std::make_unique<CommentGraphicsObject>(*this, commentId);
        
        // Restore comment properties
        commentGO->setCommentText(commentJson["text"].toString());
        
        QJsonObject posObj = commentJson["pos"].toObject();
        commentGO->setPos(posObj["x"].toDouble(), posObj["y"].toDouble());
        
        // Restore grouped nodes
        std::unordered_set<NodeId> groupedNodes;
        QJsonArray groupedNodesArray = commentJson["groupedNodes"].toArray();
        for (QJsonValue nodeVal : groupedNodesArray) {
            groupedNodes.insert(static_cast<NodeId>(nodeVal.toInteger()));
        }
        commentGO->setGroupedNodes(groupedNodes);
        
        // Add to scene
        addComment(commentId, std::move(commentGO));
    }

    Q_EMIT sceneLoaded();

    return true;
}

} // namespace QtNodes
