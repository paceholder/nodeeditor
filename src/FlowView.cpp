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
  , _cutSelectionAction(Q_NULLPTR)
  , _pasteClipboardAction(Q_NULLPTR)
  , _createGroupFromSelectionAction(Q_NULLPTR)
  , _loadGroupAction(Q_NULLPTR)
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

  connect(_clipboard, &QClipboard::dataChanged, [this]()
  {
    bool validClipboard = checkMimeFiles(_clipboard->mimeData())
                          || !mimeToJson(_clipboard->mimeData()).isEmpty();
    _pasteClipboardAction->setEnabled(validClipboard);
  });

  //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}


FlowView::
FlowView(FlowScene *scene, QWidget *parent)
  : FlowView(parent)
{
  setScene(scene);

  _scene->setSceneRect(_scene->itemsBoundingRect());

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

QAction*
FlowView::
createGroupFromSelectionAction() const
{
  return _createGroupFromSelectionAction;
}

QAction*
FlowView::
loadGroupAction() const
{
  return _loadGroupAction;
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
      auto text = mimeData->data("text/plain");
      QJsonDocument jsonDoc = QJsonDocument::fromJson(text);
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

  if (_createGroupFromSelectionAction != nullptr)
    delete _createGroupFromSelectionAction;
  _createGroupFromSelectionAction = new QAction(
    QStringLiteral("Create group from selection"), this);
  _createGroupFromSelectionAction->setEnabled(false);
  connect(_createGroupFromSelectionAction, &QAction::triggered,
          [this]()
  {
    _scene->createGroupFromSelection();
  });
  addAction(_createGroupFromSelectionAction);

  if (_loadGroupAction != nullptr)
    delete _loadGroupAction;
  _loadGroupAction = new QAction(QStringLiteral("Load Group..."), this);
  _createGroupFromSelectionAction->setEnabled(true);
  connect(_loadGroupAction, &QAction::triggered, this, &FlowView::handleLoadGroup);
  addAction(_loadGroupAction);
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

    nodeMenu.addAction(_createGroupFromSelectionAction);
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
  auto selection = _scene->selectedItems();
  clipboardData->setData(_clipboardMimeType, _scene->saveItems(selection));
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
  QPointF pastePos;
  constexpr QPointF posOffset{(double)_pastePosOffset, (double)_pastePosOffset};

  // if the paste action comes from a defined position (e.g. right-click context menu)
  if (_pasteClipboardAction->data().isValid())
  {
    pastePos = _pasteClipboardAction->data().toPointF();
  }
  else
  {
    pastePos = mapToScene(viewport()->rect().center());

    // if the viewport center hasn't changed since the last paste action
    if (pastePos == _lastPastePos)
    {
      _pasteCount++;
      pastePos += _pasteCount * posOffset;
    }
    else
    {
      _pasteCount = 0;
      _lastPastePos = pastePos;
    }
  }
  if (checkMimeFiles(_clipboard->mimeData()))
  {
    loadFilesFromMime(_clipboard->mimeData(), pastePos);
  }
  else
  {
    const QByteArray data = mimeToJson(_clipboard->mimeData());
    if (!data.isEmpty())
    {
      _scene->loadItems(data, pastePos);
    }
    else
    {
      qDebug() << "Failed to convert clipboard to json!";
    }
  }

  _pasteClipboardAction->setData(QVariant());
}

void
FlowView::
handleLoadGroup()
{
  QPointF loadPos = _loadGroupAction->data().isValid()?
                    _pasteClipboardAction->data().toPointF() :
                    mapToScene(viewport()->rect().center());

  auto groupPtr = _scene->loadGroupFile();
  if (auto group = groupPtr.lock(); group)
  {
    group->groupGraphicsObject().setPosition(loadPos);
  }
}

void
FlowView::
handleFilePaste(const QString& filepath, const QPointF& pos)
{
  if (!QFileInfo::exists(filepath))
  {
    qDebug() << "Error! Couldn't find dropped file.";
    return;
  }

  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error opening dropped file!";
    return;
  }

  QByteArray wholeFile = file.readAll();

  if (filepath.endsWith(QStringLiteral(".flow")))
  {
    clearSelectionAction()->trigger();
    _scene->loadItems(wholeFile, pos);
    return;
  }

  if (filepath.endsWith(QStringLiteral(".group")))
  {
    clearSelectionAction()->trigger();
    const QJsonObject fileJson = QJsonDocument::fromJson(wholeFile).object();
    auto groupWeakPtr = _scene->restoreGroup(fileJson).first;
    if (auto groupPtr = groupWeakPtr.lock(); groupPtr)
    {
      auto& ggoRef = groupPtr->groupGraphicsObject();
      ggoRef.setPosition(pos);
      ggoRef.setSelected(true);
    }
  }

}

void
FlowView::
loadFilesFromMime(const QMimeData *mimeData, const QPointF &pos)
{
  if (mimeData->hasUrls())
  {
    QPointF dropPosOffset{(double)_pastePosOffset, (double)_pastePosOffset};
    int fileCounter{0};

    auto urls = mimeData->urls();
    for (const auto& url : urls)
    {
      handleFilePaste(url.toLocalFile(), pos + fileCounter * dropPosOffset);
      fileCounter++;
    }
  }
}

bool
FlowView::
checkMimeFiles(const QMimeData *mimeData) const
{
  if (mimeData->hasUrls())
  {
    auto urls = mimeData->urls();
    for (const auto& url : urls)
    {
      auto filepath = url.toLocalFile();
      if (filepath.endsWith(QStringLiteral(".flow")) ||
          filepath.endsWith(QStringLiteral(".group")))
        return true;
    }
  }
  return false;
}

void
FlowView::
gentleZoom(double factor)
{
  double newFactor{_currentZoomFactor * factor};
  if (newFactor < _zoomLimits.first || newFactor > _zoomLimits.second)
    return;
  _currentZoomFactor = newFactor;
  scale(factor, factor);
}

void
FlowView::
contextMenuEvent(QContextMenuEvent *event)
{
  auto menuPos{mapToScene(event->pos())};
  auto menuPosVariant{QVariant(menuPos)};

  _pasteClipboardAction->setData(menuPosVariant);
  _loadGroupAction->setData(menuPosVariant);

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

      node.nodeGraphicsObject().setPos(menuPos);

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

  if (_scene->checkCopyableSelection())
  {
    modelMenu.addAction(_createGroupFromSelectionAction);
  }

  modelMenu.addAction(_loadGroupAction);
  modelMenu.addAction(_copySelectionAction);
  modelMenu.addAction(_cutSelectionAction);
  modelMenu.addAction(_pasteClipboardAction);

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

  modelMenu.exec(event->globalPos());
  _pasteClipboardAction->setData(QVariant());
  _loadGroupAction->setData(QVariant());
}


void
FlowView::
wheelEvent(QWheelEvent *event)
{
  auto delta = event->angleDelta().y();

  if (delta == 0)
  {
    event->ignore();
    return;
  }
  delta > 0 ? scaleUp() : scaleDown();
}


void
FlowView::
scaleUp()
{
  ViewportAnchor anchor{transformationAnchor()};
  gentleZoom(_zoomFactor);
  setTransformationAnchor(anchor);
}


void
FlowView::
scaleDown()
{
  gentleZoom(1.0 / _zoomFactor);

  auto newSceneRect = mapToScene(viewport()->rect()).boundingRect();
  newSceneRect |= scene()->sceneRect();
  scene()->setSceneRect(newSceneRect);
}


void
FlowView::
deleteSelectedNodes()
{

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

  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if (auto g = qgraphicsitem_cast<GroupGraphicsObject*>(item))
    {
      _scene->removeGroup(g->group().id());
    }
  }
}

void
FlowView::
handleSelectionChanged()
{
  bool isSelectionCopyable = _scene->checkCopyableSelection();
  if (_copySelectionAction != nullptr)
    _copySelectionAction->setEnabled(isSelectionCopyable);
  if (_cutSelectionAction != nullptr)
    _cutSelectionAction->setEnabled(isSelectionCopyable);
  if (_createGroupFromSelectionAction != nullptr)
    _createGroupFromSelectionAction->setEnabled(isSelectionCopyable);
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

    auto modifiers = QApplication::keyboardModifiers();
    auto* selectedItem = _scene->itemAt(_clickPos, QTransform());

    if (selectedItem != nullptr &&
        selectedItem->isEnabled() &&
        (modifiers & Qt::ControlModifier) &&
        (selectedItem->flags() & QGraphicsItem::ItemIsSelectable))
    {
      selectedItem->setSelected(!selectedItem->isSelected());
    }
  }
}

void
FlowView::
mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);
  if (scene()->mouseGrabberItem() == nullptr &&
      event->buttons() == Qt::LeftButton)
  {
    // Make sure shift is not being pressed
    if ((event->modifiers() & Qt::ShiftModifier) == 0)
    {
      auto mouseScenePos = mapToScene(event->pos());
      QPointF clickDiff = _clickPos - mouseScenePos;
      auto newSceneRect = mapToScene(viewport()->rect()).boundingRect();
      newSceneRect.translate(clickDiff.x(), clickDiff.y());
      newSceneRect |= scene()->sceneRect();
      scene()->setSceneRect(newSceneRect);
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
//  _scene->setSceneRect(this->rect());
  QGraphicsView::showEvent(event);
}

void
FlowView::
dragEnterEvent(QDragEnterEvent *event)
{
  if (checkMimeFiles(event->mimeData()))
  {
    event->acceptProposedAction();
    return;
  }

  // here we copy the relevant data to a local object because the reference
  // to the event's mime data was being lost when calling mimeToJson.
  QMimeData* mimeData = new QMimeData;

  // retrieves only the data relevant to the scene
  QList<QString> relevantTypes = {_clipboardMimeType, "text/plain"};
  for (const auto& format : relevantTypes)
  {
    if (event->mimeData()->hasFormat(format))
    {
      auto data = event->mimeData()->data(format);
      mimeData->setData(format, data);
    }
  }

  if (event->proposedAction() == Qt::CopyAction &&
      !mimeToJson(mimeData).isEmpty())
  {
    event->acceptProposedAction();
  }
}

void
FlowView::
dragMoveEvent(QDragMoveEvent *event)
{
  event->acceptProposedAction();
}

void
FlowView::
dropEvent(QDropEvent *event)
{
  auto dropPos = mapToScene(event->pos());

  // if files are being dropped
  if (event->mimeData()->hasUrls())
  {
    loadFilesFromMime(event->mimeData(), dropPos);
    event->acceptProposedAction();
  }

  // if a json object (or a plain text describing it) is being dropped
  auto droppedJson = mimeToJson(event->mimeData());
  if (!droppedJson.isEmpty())
  {
    event->acceptProposedAction();
    _scene->loadItems(droppedJson, dropPos);
  }
}


FlowScene *
FlowView::
scene()
{
  return _scene;
}
