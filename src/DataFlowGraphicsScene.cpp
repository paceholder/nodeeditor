#include "DataFlowGraphicsScene.hpp"

#include "CommentGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "GraphicsView.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "NodeGraphicsObject.hpp"
#include "UndoCommands.hpp"

#include <QtWidgets/QCheckBox>
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
#include <unordered_set>
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

    // Add context sensitive checkbox
    auto *contextCheckBox = new QCheckBox(QStringLiteral("Context Sensitive"), modelMenu);
    contextCheckBox->setChecked(true); // Default to context sensitive
    
    auto *checkBoxAction = new QWidgetAction(modelMenu);
    checkBoxAction->setDefaultWidget(contextCheckBox);
    
    // 2.
    modelMenu->addAction(checkBoxAction);
    modelMenu->addSeparator();

    // Add result treeview to the context menu
    QTreeWidget *treeView = new QTreeWidget(modelMenu);
    treeView->header()->close();

    auto *treeViewAction = new QWidgetAction(modelMenu);
    treeViewAction->setDefaultWidget(treeView);

    // 2.
    modelMenu->addAction(treeViewAction);

    auto registry = _graphModel.dataModelRegistry();

    // Build initial tree with all nodes
    std::unordered_map<QString, QTreeWidgetItem*> categoryItems;
    for (auto const &cat : registry->categories()) {
        auto item = new QTreeWidgetItem(treeView);
        item->setText(0, cat);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        categoryItems[cat] = item;
    }

    // Store node items for filtering
    QList<QTreeWidgetItem*> allNodeItems;
    
    for (auto const &assoc : registry->registeredModelsCategoryAssociation()) {
        auto categoryIt = categoryItems.find(assoc.second);
        if (categoryIt == categoryItems.end())
            continue;

        auto item = new QTreeWidgetItem(categoryIt->second);
        item->setText(0, assoc.first);
        allNodeItems.append(item);
    }

    treeView->expandAll();
    
    // Function to apply context filtering
    auto applyContextFilter = [this, registry, allNodeItems, categoryItems, treeView, contextCheckBox]() {
        // Always show all nodes if context sensitivity is disabled
        if (!contextCheckBox->isChecked()) {
            for (auto item : allNodeItems) {
                item->setHidden(false);
            }
            for (auto const& [cat, item] : categoryItems) {
                item->setHidden(false);
            }
            return;
        }
        
        // Check if we have a pending connection
        ConnectionId pendingConn = pendingConnection();
        
        // If no pending connection (right-click on empty area), show all nodes
        if (pendingConn.inNodeId == InvalidNodeId && pendingConn.outNodeId == InvalidNodeId) {
            for (auto item : allNodeItems) {
                item->setHidden(false);
            }
            for (auto const& [cat, item] : categoryItems) {
                item->setHidden(false);
            }
            return;
        }
        
        // We have a pending connection, apply filtering
        bool needInputPort = (pendingConn.outNodeId != InvalidNodeId);
        bool needOutputPort = (pendingConn.inNodeId != InvalidNodeId);
        
        // Try to get the data type from the pending connection
        QVariant dataTypeVariant;
        if (needInputPort) {
            // We have an output connection, need compatible input
            dataTypeVariant = _graphModel.portData(pendingConn.outNodeId, 
                                                   PortType::Out, 
                                                   pendingConn.outPortIndex, 
                                                   PortRole::DataType);
        } else if (needOutputPort) {
            // We have an input connection, need compatible output
            dataTypeVariant = _graphModel.portData(pendingConn.inNodeId, 
                                                   PortType::In, 
                                                   pendingConn.inPortIndex, 
                                                   PortRole::DataType);
        }
        
        // Check if we successfully got a data type
        if (!dataTypeVariant.isValid() || !dataTypeVariant.canConvert<NodeDataType>()) {
            // Couldn't get data type, show all nodes as fallback
            for (auto item : allNodeItems) {
                item->setHidden(false);
            }
            for (auto const& [cat, item] : categoryItems) {
                item->setHidden(false);
            }
            return;
        }
        
        NodeDataType pendingDataType = dataTypeVariant.value<NodeDataType>();
        
        // Filter nodes based on compatibility
        std::unordered_set<QString> visibleCategories;
        for (auto item : allNodeItems) {
            QString modelName = item->text(0);
            auto model = registry->create(modelName);
            if (!model) {
                item->setHidden(true);
                continue;
            }
            
            bool compatible = false;
            
            try {
                if (needInputPort) {
                    // Check if this node has compatible input ports
                    unsigned int numInputs = model->nPorts(PortType::In);
                    if (numInputs == 0) {
                        // Node has no input ports, can't connect
                        compatible = false;
                    } else {
                        for (unsigned int i = 0; i < numInputs; ++i) {
                            auto nodeDataType = model->dataType(PortType::In, i);
                            if (nodeDataType.id == pendingDataType.id) {
                                compatible = true;
                                break;
                            }
                        }
                    }
                } else if (needOutputPort) {
                    // Check if this node has compatible output ports
                    unsigned int numOutputs = model->nPorts(PortType::Out);
                    if (numOutputs == 0) {
                        // Node has no output ports, can't connect
                        compatible = false;
                    } else {
                        for (unsigned int i = 0; i < numOutputs; ++i) {
                            auto nodeDataType = model->dataType(PortType::Out, i);
                            if (nodeDataType.id == pendingDataType.id) {
                                compatible = true;
                                break;
                            }
                        }
                    }
                }
            } catch (...) {
                // If there's any error checking compatibility, treat as incompatible
                compatible = false;
            }
            
            item->setHidden(!compatible);
            if (compatible && item->parent()) {
                visibleCategories.insert(item->parent()->text(0));
            }
        }
        
        // Hide empty categories
        for (auto const& [cat, item] : categoryItems) {
            item->setHidden(visibleCategories.find(cat) == visibleCategories.end());
        }
    };
    
    // Apply initial context filter
    applyContextFilter();
    
    // Connect checkbox to reapply filter
    connect(contextCheckBox, &QCheckBox::toggled, applyContextFilter);

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
    connect(txtBox, &QLineEdit::textChanged, [treeView, applyContextFilter, allNodeItems, categoryItems, contextCheckBox](const QString &text) {
        // First apply context filter
        applyContextFilter();
        
        // Then apply text filter on top
        if (text.isEmpty()) {
            return; // Context filter already applied
        }
        
        // Hide all categories first
        for (auto const& [cat, item] : categoryItems) {
            item->setHidden(true);
        }
        
        // Apply text filter to visible items only
        for (auto item : allNodeItems) {
            if (!item->isHidden()) { // Only filter items that passed context filter
                auto modelName = item->text(0);
                const bool match = modelName.contains(text, Qt::CaseInsensitive);
                item->setHidden(!match);
                if (match && item->parent()) {
                    item->parent()->setHidden(false);
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
