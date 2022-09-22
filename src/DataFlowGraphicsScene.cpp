#include "DataFlowGraphicsScene.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "GraphicsView.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "NodeGraphicsObject.hpp"

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


namespace QtNodes
{


DataFlowGraphicsScene::
DataFlowGraphicsScene(DataFlowGraphModel& graphModel,
                      QObject*            parent)
  : BasicGraphicsScene(graphModel, parent)
  , _graphModel(graphModel)
{
  connect(&_graphModel, &DataFlowGraphModel::inPortDataWasSet,
          this, &DataFlowGraphicsScene::onPortDataSet);
}


// TODO constructor for an empyt scene?


//---------------------------------------------------------------------

std::vector<NodeId>
DataFlowGraphicsScene::
selectedNodes() const
{
  QList<QGraphicsItem*> graphicsItems = selectedItems();

  std::vector<NodeId> result;
  result.reserve(graphicsItems.size());

  for (QGraphicsItem* item : graphicsItems)
  {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

    if (ngo != nullptr)
    {
      result.push_back(ngo->nodeId());
    }
  }

  return result;
}


QMenu*
DataFlowGraphicsScene::
createSceneMenu(QPointF const scenePos)
{
  QMenu* modelMenu = new QMenu();

  auto skipText = QStringLiteral("skip me");

  // Add filterbox to the context menu
  auto* txtBox = new QLineEdit(modelMenu);
  txtBox->setPlaceholderText(QStringLiteral("Filter"));
  txtBox->setClearButtonEnabled(true);

  auto* txtBoxAction = new QWidgetAction(modelMenu);
  txtBoxAction->setDefaultWidget(txtBox);

  // 1.
  modelMenu->addAction(txtBoxAction);

  // Add result treeview to the context menu
  QTreeWidget* treeView = new QTreeWidget(modelMenu);
  treeView->header()->close();

  auto* treeViewAction = new QWidgetAction(modelMenu);
  treeViewAction->setDefaultWidget(treeView);

  // 2.
  modelMenu->addAction(treeViewAction);

  auto registry = _graphModel.dataModelRegistry();

  QMap<QString, QTreeWidgetItem*> topLevelItems;
  for (auto const& cat : registry->categories())
  {
    auto item = new QTreeWidgetItem(treeView);
    item->setText(0, cat);
    item->setData(0, Qt::UserRole, skipText);
    topLevelItems[cat] = item;
  }

  for (auto const& assoc : registry->registeredModelsCategoryAssociation())
  {
    auto parent = topLevelItems[assoc.second];
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, assoc.first);
    item->setData(0, Qt::UserRole, assoc.first);
  }

  treeView->expandAll();

  connect(treeView, &QTreeWidget::itemClicked,
          [this,
           modelMenu,
           skipText,
           scenePos](QTreeWidgetItem* item, int)
          {
            QString modelName = item->data(0, Qt::UserRole).toString();

            if (modelName == skipText)
            {
              return;
            }

            NodeId nodeId = this->_graphModel.addNode(modelName);

            if (nodeId != InvalidNodeId)
            {
              _graphModel.setNodeData(nodeId,
                                      NodeRole::Position,
                                      scenePos);
            }

            modelMenu->close();
          });

  //Setup filtering
  connect(txtBox, &QLineEdit::textChanged,
          [&](const QString& text)
          {
            for (auto& topLvlItem : topLevelItems)
            {
              for (int i = 0; i < topLvlItem->childCount(); ++i)
              {
                auto child = topLvlItem->child(i);
                auto modelName = child->data(0, Qt::UserRole).toString();
                const bool match = (modelName.contains(text, Qt::CaseInsensitive));
                child->setHidden(!match);
              }
            }
          });

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

  // QMenu's instance auto-destruction
  modelMenu->setAttribute(Qt::WA_DeleteOnClose);

  return modelMenu;
}


void
DataFlowGraphicsScene::
save() const
{
  QString fileName =
    QFileDialog::getSaveFileName(nullptr,
                                 tr("Open Flow Scene"),
                                 QDir::homePath(),
                                 tr("Flow Scene Files (*.flow)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith("flow", Qt::CaseInsensitive))
      fileName += ".flow";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(saveToJsonDocument().toJson());
    }
  }
}


void
DataFlowGraphicsScene::
load()
{
  QString fileName =
    QFileDialog::getOpenFileName(nullptr,
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

  loadFromJsonDocument(QJsonDocument::fromJson(wholeFile));
}


QJsonDocument
DataFlowGraphicsScene::
saveToJsonDocument() const
{
  return _graphModel.save();
}


void
DataFlowGraphicsScene::
loadFromJsonDocument(QJsonDocument const & json)
{
  _graphModel.load(json);
}



void
DataFlowGraphicsScene::
onPortDataSet(NodeId const    nodeId,
              PortType const  portType,
              PortIndex const portIndex)
{
  Q_UNUSED(portType);
  Q_UNUSED(portIndex);

  // From BasicGraphicsScene
  onNodeUpdated(nodeId);
}


}
