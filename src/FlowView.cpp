#include "FlowView.hpp"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtWidgets/QMenu>

#include <QtCore/QRectF>
#include <QtCore/QPointF>

#include <QtOpenGL>
#include <QtWidgets>

#include <QDebug>
#include <iostream>
#include <cmath>

#include "FlowScene.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "StyleCollection.hpp"
#include "NodeGroup.hpp"
#include "GroupGraphicsObject.hpp"

using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::GroupGraphicsObject;
using QtNodes::NodeGraphicsObject;

const QString FlowView::_clipboardMimeType = "application/json";

FlowView::
FlowView(QWidget *parent)
  : QGraphicsView(parent)
  , _clearSelectionAction(Q_NULLPTR)
  , _deleteSelectionAction(Q_NULLPTR)
  , _copySelectionAction(Q_NULLPTR)
  , _pasteClipboardAction(Q_NULLPTR)
  , _scene(Q_NULLPTR)
  , _clipboard(QApplication::clipboard())
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);
  setRenderHint(QPainter::TextAntialiasing);
  setRenderHint(QPainter::SmoothPixmapTransform);

  auto const &flowViewStyle = StyleCollection::flowViewStyle();

  setBackgroundBrush(flowViewStyle.BackgroundColor);

  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//  setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);

  setAcceptDrops(true);

  //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}


FlowView::
FlowView(FlowScene *scene, QWidget *parent)
  : FlowView(parent)
{
  setScene(scene);

  connect(_scene, &FlowScene::selectionChanged, this,
          &FlowView::handleSelectionChanged);
}


QAction*
FlowView::
clearSelectionAction() const
{
  return _clearSelectionAction;
}


QAction*
FlowView::
deleteSelectionAction() const
{
  return _deleteSelectionAction;
}

QAction*
FlowView::
copySelectionAction() const
{
  return _copySelectionAction;
}

QAction*
FlowView::
cutSelectionAction() const
{
  return _cutSelectionAction;
}

QAction*
FlowView::
pasteClipboardAction() const
{
  return _pasteClipboardAction;
}

QByteArray
FlowView::
mimeToJson(const QMimeData *mimeData) const
{
  if (mimeData != nullptr)
  {
    if (mimeData->hasFormat(_clipboardMimeType))
    {
      return mimeData->data(_clipboardMimeType);
    }
    if (mimeData->hasText())
    {
      QJsonDocument jsonDoc = QJsonDocument::fromJson(mimeData->text().toUtf8());
      if (!jsonDoc.isNull())
      {
        return jsonDoc.toJson();
      }
    }
  }

  return QByteArray();
}

void
FlowView::
setScene(FlowScene *scene)
{
  _scene = scene;
  QGraphicsView::setScene(_scene);

  // setup actions
  if (_clearSelectionAction != nullptr)
    delete _clearSelectionAction;
  _clearSelectionAction = new QAction(QStringLiteral("Clear Selection"), this);
  _clearSelectionAction->setShortcut(Qt::Key_Escape);
  connect(_clearSelectionAction, &QAction::triggered, _scene, &QGraphicsScene::clearSelection);
  addAction(_clearSelectionAction);

  if (_deleteSelectionAction != nullptr)
    delete _deleteSelectionAction;
  _deleteSelectionAction = new QAction(QStringLiteral("Delete Selection"), this);
  _deleteSelectionAction->setShortcut(Qt::Key_Delete);
  connect(_deleteSelectionAction, &QAction::triggered, this, &FlowView::deleteSelectedNodes);
  addAction(_deleteSelectionAction);

  if (_copySelectionAction != nullptr)
    delete _copySelectionAction;
  _copySelectionAction = new QAction(QStringLiteral("Copy"), this);
  _copySelectionAction->setShortcut(QKeySequence::Copy);
  _copySelectionAction->setEnabled(false);
  connect(_copySelectionAction, &QAction::triggered, this, &FlowView::copySelectionToClipboard);
  addAction(_copySelectionAction);

  if (_cutSelectionAction != nullptr)
    delete _cutSelectionAction;
  _cutSelectionAction = new QAction(QStringLiteral("Cut"), this);
  _cutSelectionAction->setShortcut(QKeySequence::Cut);
  _cutSelectionAction->setEnabled(false);
  connect(_cutSelectionAction, &QAction::triggered, this, &FlowView::cutSelectionToClipboard);
  addAction(_cutSelectionAction);

  if (_pasteClipboardAction != nullptr)
    delete _pasteClipboardAction;
  _pasteClipboardAction = new QAction(QStringLiteral("Paste"), this);
  _pasteClipboardAction->setShortcut(QKeySequence::Paste);
  _pasteClipboardAction->setEnabled(false);
  connect(_pasteClipboardAction, &QAction::triggered, this, &FlowView::pasteFromClipboard);
  addAction(_pasteClipboardAction);
}

void
FlowView::
groupContextMenu(QContextMenuEvent* event,
                 GroupGraphicsObject* ggo)
{
  QMenu groupMenu;

  auto* saveGroupAction = new QAction(&groupMenu);
  saveGroupAction->setText(QStringLiteral("Save Group..."));
  groupMenu.addAction(saveGroupAction);
  groupMenu.addAction(_copySelectionAction);
  groupMenu.addAction(_cutSelectionAction);

  connect(saveGroupAction, &QAction::triggered,
          [&ggo, &_scene = _scene]()
  {
    _scene->saveGroupFile(ggo->group().id());
  });

  groupMenu.exec(event->globalPos());
}

void
FlowView::
nodeContextMenu(QContextMenuEvent* event,
                NodeGraphicsObject* ngo)
{
  QMenu nodeMenu;

  if (ngo->node().isInGroup())
  {
    if (auto nodeGroup = ngo->node().nodeGroup().lock(); nodeGroup)
    {
      auto* removeFromGroupAction = new QAction(&nodeMenu);
      removeFromGroupAction->setText(QString("Remove from \"" + nodeGroup->name()) + "\"");
      nodeMenu.addAction(removeFromGroupAction);

      connect(removeFromGroupAction, &QAction::triggered, [this, &ngo]
      {
        _scene->removeNodeFromGroup(ngo->node().id());
      });
    }
  }
  else
  {
    auto* groupsMenu = new QMenu(&nodeMenu);
    QList<QAction*> groupActions{};
    groupActions.reserve(_scene->groups().size());
    groupsMenu->setTitle(QStringLiteral("Add to group..."));
    for (const auto& groupEntry : _scene->groups())
    {
      auto* currentGroupAction = new QAction(&nodeMenu);
      currentGroupAction->setText(groupEntry.second->name());

      connect(currentGroupAction, &QAction::triggered,
              [&_scene = _scene, groupEntry, &ngo]
      {
        _scene->addNodeToGroup(ngo->node().id(), groupEntry.second->id());
        for (const auto& group : _scene->groups())
        {
          group.second->groupGraphicsObject().setHovered(false);
        }
      });

      connect(currentGroupAction, &QAction::hovered,
              [groupEntry, &_scene = _scene]()
      {
        for (const auto& group : _scene->groups())
        {
          group.second->groupGraphicsObject().setHovered(group == groupEntry);
        }
      });

      groupActions.push_back(currentGroupAction);
    }
    groupsMenu->addActions(groupActions);
    groupsMenu->setEnabled(!groupActions.empty());
    nodeMenu.addMenu(groupsMenu);

    auto createGroupAction = new QAction(&nodeMenu);
    createGroupAction->setText(QStringLiteral("Create group"));
    auto createGroup = [&_scene = _scene, &ngo]()
    {
      auto nodes = _scene->selectedNodes();
      Node* currentNode = &ngo->node();
      if (std::find(nodes.begin(), nodes.end(), currentNode) == nodes.end())
      {
        nodes.push_back(currentNode);
      }
      _scene->createGroup(nodes);
    };
    connect(createGroupAction, &QAction::triggered, createGroup);

    nodeMenu.addAction(createGroupAction);
  }
  nodeMenu.addAction(_copySelectionAction);
  nodeMenu.addAction(_cutSelectionAction);
  nodeMenu.exec(event->globalPos());
}

void
FlowView::
copySelectionToClipboard()
{
  QMimeData* clipboardData = new QMimeData;
  clipboardData->setData(_clipboardMimeType, _scene->saveSelectedItems());
  _clipboard->setMimeData(clipboardData);
  _pasteClipboardAction->setEnabled(!clipboardData->data(_clipboardMimeType).isEmpty());
}

void
FlowView::
cutSelectionToClipboard()
{
  copySelectionToClipboard();
  deleteSelectionAction()->trigger();
}

void
FlowView::
pasteFromClipboard()
{
  QPointF paste_pos = _pasteClipboardAction->data().isValid()?
                      _pasteClipboardAction->data().toPointF()
                      : mapToScene(viewport()->rect().center());

  const QByteArray data = mimeToJson(_clipboard->mimeData());
  if (!data.isEmpty())
  {
    _scene->pasteItems(data, paste_pos);
  }
  else
  {
    qDebug() << "Failed to convert clipboard to json!";
  }

  _pasteClipboardAction->setData(QVariant());
}

void
FlowView::
contextMenuEvent(QContextMenuEvent *event)
{
  _pasteClipboardAction->setEnabled(!mimeToJson(_clipboard->mimeData()).isEmpty());
  _pasteClipboardAction->setData(QVariant(mapToScene(event->pos())));

  auto clickedItem = itemAt(event->pos());
  if (clickedItem)
  {
    clickedItem->setSelected(true);
    if (auto groupGO = qgraphicsitem_cast<GroupGraphicsObject*>(clickedItem); groupGO)
      groupContextMenu(event, groupGO);
    else if (auto nodeGO = qgraphicsitem_cast<NodeGraphicsObject*>(clickedItem); nodeGO)
      nodeContextMenu(event, nodeGO);
    _pasteClipboardAction->setData(QVariant());
    return;
  }

  QMenu modelMenu;

  auto skipText = QStringLiteral("skip me");

  //Add filterbox to the context menu
  auto *txtBox = new QLineEdit(&modelMenu);

  txtBox->setPlaceholderText(QStringLiteral("Filter"));
  txtBox->setClearButtonEnabled(true);

  auto *txtBoxAction = new QWidgetAction(&modelMenu);
  txtBoxAction->setDefaultWidget(txtBox);

  modelMenu.addAction(txtBoxAction);

  //Add result treeview to the context menu
  auto *treeView = new QTreeWidget(&modelMenu);
  treeView->header()->close();

  auto *treeViewAction = new QWidgetAction(&modelMenu);
  treeViewAction->setDefaultWidget(treeView);

  modelMenu.addAction(treeViewAction);

  QMap<QString, QTreeWidgetItem*> topLevelItems;
  for (auto const &cat : _scene->registry().categories())
  {
    auto item = new QTreeWidgetItem(treeView);
    item->setText(0, cat);
    item->setData(0, Qt::UserRole, skipText);
    topLevelItems[cat] = item;
  }

  for (auto const &assoc : _scene->registry().registeredModelsCategoryAssociation())
  {
    auto parent = topLevelItems[assoc.second];
    auto item   = new QTreeWidgetItem(parent);
    item->setText(0, assoc.first);
    item->setData(0, Qt::UserRole, assoc.first);
  }

  treeView->expandAll();

  QPoint pos = event->pos();
  QPointF posView = this->mapToScene(pos);

  connect(treeView, &QTreeWidget::itemClicked, [&](QTreeWidgetItem *item, int)
  {
    QString modelName = item->data(0, Qt::UserRole).toString();

    if (modelName == skipText)
    {
      return;
    }

    auto type = _scene->registry().create(modelName);

    if (type)
    {
      auto& node = _scene->createNode(std::move(type));

      node.nodeGraphicsObject().setPos(posView);

      _scene->nodePlaced(node);
    }
    else
    {
      qDebug() << "Model not found";
    }

    modelMenu.close();
  });

  //Setup filtering
  connect(txtBox, &QLineEdit::textChanged, [&](const QString &text)
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

  auto nodes = _scene->selectedNodes();
  if (!nodes.empty())
  {
    auto createGroupAction = new QAction(&modelMenu);
    createGroupAction->setText(QStringLiteral("Create group from selection"));
    connect(createGroupAction, &QAction::triggered,
            [&_scene = _scene, &nodes]()
    {
      _scene->createGroup(nodes);
    });
    modelMenu.addAction(createGroupAction);
  }

  auto restoreGroupAction = new QAction(&modelMenu);
  restoreGroupAction->setText(QStringLiteral("Load Group..."));
  connect(restoreGroupAction, &QAction::triggered,
          [&_scene = _scene, &posView]()
  {
    std::weak_ptr<NodeGroup> groupPtr = _scene->loadGroupFile();
    if (auto group = groupPtr.lock(); group)
    {
      group->groupGraphicsObject().setPosition(posView);
    }
  });
  modelMenu.addAction(restoreGroupAction);
  modelMenu.addAction(_copySelectionAction);
  modelMenu.addAction(_cutSelectionAction);
  modelMenu.addAction(_pasteClipboardAction);

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

  modelMenu.exec(event->globalPos());
  _pasteClipboardAction->setData(QVariant());
}


void
FlowView::
wheelEvent(QWheelEvent *event)
{
  QPoint delta = event->angleDelta();

  if (delta.y() == 0)
  {
    event->ignore();
    return;
  }

  double const d = delta.y() / std::abs(delta.y());

  if (d > 0.0)
    scaleUp();
  else
    scaleDown();
}


void
FlowView::
scaleUp()
{
  double const step   = 1.2;
  double const factor = std::pow(step, 1.0);

  QTransform t = transform();

  if (t.m11() > 2.0)
    return;

  scale(factor, factor);
}


void
FlowView::
scaleDown()
{
  double const step   = 1.2;
  double const factor = std::pow(step, -1.0);

  scale(factor, factor);
}


void
FlowView::
deleteSelectedNodes()
{

  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto g = qgraphicsitem_cast<GroupGraphicsObject*>(item))
    {
      _scene->removeGroup(g->group().id());
    }
  }
  // Delete the selected connections first, ensuring that they won't be
  // automatically deleted when selected nodes are deleted (deleting a node
  // deletes some connections as well)
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
      _scene->deleteConnection(c->connection());
  }

  // Delete the nodes; this will delete many of the connections.
  // Selected connections were already deleted prior to this loop, otherwise
  // qgraphicsitem_cast<NodeGraphicsObject*>(item) could be a use-after-free
  // when a selected connection is deleted by deleting the node.
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
    {
      _scene->removeNode(n->node());
    }
  }
}

void
FlowView::
handleSelectionChanged()
{
  if (_copySelectionAction != nullptr && _cutSelectionAction != nullptr)
  {
    bool isSelectionCopyable = _scene->checkCopyableSelection();
    _copySelectionAction->setEnabled(isSelectionCopyable);
    _cutSelectionAction->setEnabled(isSelectionCopyable);
  }
}


void
FlowView::
keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Shift:
    setDragMode(QGraphicsView::RubberBandDrag);
    break;

  default:
    break;
  }

  QGraphicsView::keyPressEvent(event);
}


void
FlowView::
keyReleaseEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Shift:
    setDragMode(QGraphicsView::ScrollHandDrag);
    break;

  default:
    break;
  }
  QGraphicsView::keyReleaseEvent(event);
}


void
FlowView::
mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);
  if (event->button() == Qt::LeftButton)
  {
    _clickPos = mapToScene(event->pos());
  }
}


void
FlowView::
mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);
  if (scene()->mouseGrabberItem() == nullptr && event->buttons() == Qt::LeftButton)
  {
    // Make sure shift is not being pressed
    if ((event->modifiers() & Qt::ShiftModifier) == 0)
    {
      QPointF difference = _clickPos - mapToScene(event->pos());
      setSceneRect(sceneRect().translated(difference.x(), difference.y()));
    }
  }
}


void
FlowView::
drawBackground(QPainter* painter, const QRectF& r)
{
  QGraphicsView::drawBackground(painter, r);

  auto drawGrid =
    [&](double gridStep)
  {
    QRect   windowRect = rect();
    QPointF tl = mapToScene(windowRect.topLeft());
    QPointF br = mapToScene(windowRect.bottomRight());

    double left   = std::floor(tl.x() / gridStep - 0.5);
    double right  = std::floor(br.x() / gridStep + 1.0);
    double bottom = std::floor(tl.y() / gridStep - 0.5);
    double top    = std::floor (br.y() / gridStep + 1.0);

    // vertical lines
    for (int xi = int(left); xi <= int(right); ++xi)
    {
      QLineF line(xi * gridStep, bottom * gridStep,
                  xi * gridStep, top * gridStep );

      painter->drawLine(line);
    }

    // horizontal lines
    for (int yi = int(bottom); yi <= int(top); ++yi)
    {
      QLineF line(left * gridStep, yi * gridStep,
                  right * gridStep, yi * gridStep );
      painter->drawLine(line);
    }
  };

  auto const &flowViewStyle = StyleCollection::flowViewStyle();

  QBrush bBrush = backgroundBrush();

  QPen pfine(flowViewStyle.FineGridColor, 1.0);

  painter->setPen(pfine);
  drawGrid(15);

  QPen p(flowViewStyle.CoarseGridColor, 1.0);

  painter->setPen(p);
  drawGrid(150);
}


void
FlowView::
showEvent(QShowEvent *event)
{
  _scene->setSceneRect(this->rect());
  QGraphicsView::showEvent(event);
}

void
FlowView::
dragEnterEvent(QDragEnterEvent *event)
{
  if (!mimeToJson(event->mimeData()).isEmpty())
  {
    event->acceptProposedAction();
  }
}

void
FlowView::
dropEvent(QDropEvent *event)
{
  if (!mimeToJson(event->mimeData()).isEmpty())
  {

    event->acceptProposedAction();
  }
}


FlowScene *
FlowView::
scene()
{
  return _scene;
}
