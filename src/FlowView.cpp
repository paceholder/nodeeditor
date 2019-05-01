#include "FlowView.hpp"

#include <QtWidgets/QGraphicsScene>
#include <QClipboard>
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
#include "ModelSelectionWidget.hpp"

using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::ModelSelectionWidget;


static auto const SkipText = QStringLiteral("skip me");

FlowView::
FlowView(QWidget *parent)
  : QGraphicsView(parent)
  , _clearSelectionAction(Q_NULLPTR)
  , _deleteSelectionAction(Q_NULLPTR)
  , _duplicateSelectionAction(Q_NULLPTR)
  , _copymultiplenodes(Q_NULLPTR)
  , _pastemultiplenodes(Q_NULLPTR)
  , _undoAction(Q_NULLPTR)
  , _redoAction(Q_NULLPTR)
  , _scene(Q_NULLPTR)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);

  auto const &flowViewStyle = StyleCollection::flowViewStyle();

  setBackgroundBrush(flowViewStyle.BackgroundColor);

  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  //setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);
}


FlowView::
FlowView(FlowScene *scene, QWidget *parent)
  : FlowView(parent)
{
  setScene(scene);
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
duplicateSelectionAction() const
{
   return _duplicateSelectionAction;
}

QAction*
FlowView::
pastemultiplenodes() const
{
   return _pastemultiplenodes;
}

QAction*
FlowView::
undoAction() const
{
   return _undoAction;
}

QAction*
FlowView::
redoAction() const
{
   return _redoAction;
}

void
FlowView::
duplicateSelectedNode()
{
   //Get Bounds of all the selected items
   double minx = std::numeric_limits<double>::max();
   double miny = std::numeric_limits<double>::max();
   double maxx = std::numeric_limits<double>::min();
   double maxy = std::numeric_limits<double>::min();
   for (QGraphicsItem * item : _scene->selectedItems())
   {
      if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
      {
         QPointF pos = n->pos();
         minx = std::min(minx, pos.x());
         miny = std::min(miny, pos.y());
         maxx = std::max(maxx, pos.x());
         maxy = std::max(maxy, pos.y());
      }
   }

   //compute centroid
   double centroidX = (maxx - minx) / 2.0 + minx;
   double centroidY = (maxy - miny) / 2.0 + miny;
   QPointF centroid(centroidX, centroidY);

   //create nodes
   std::vector<Node*> createdNodes;
   std::vector<Node*> counterpartNode;
   std::vector<QJsonObject> nodeData;
   for (QGraphicsItem * item : _scene->selectedItems())
   {
      if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
      {
         QString modelName = n->node().nodeDataModel()->name();
         auto type = _scene->registry().create(modelName);
         auto& typeRef = type;

         if (typeRef)
         {
            auto& node = _scene->createNode(std::move(typeRef));
            node.nodeDataModel()->restore(n->node().nodeDataModel()->save());
            createdNodes.push_back(&node);
            counterpartNode.push_back(&(n->node()));

            QPoint viewPointMouse = this->mapFromGlobal(QCursor::pos());
            QPointF posViewMouse = this->mapToScene(viewPointMouse);

            QPointF pos = posViewMouse + (n->pos() - centroid);

            node.nodeGraphicsObject().setPos(pos);
         }
      }
   }

   //create connections
   std::vector<std::shared_ptr<Connection> > createdConnections;
   for (QGraphicsItem * item : _scene->selectedItems())
   {
      if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
      {
         Node* nodeIn = c->connection().getNode(PortType::In);
         PortIndex portIndexIn = c->connection().getPortIndex(PortType::In);
         Node* nodeOut = c->connection().getNode(PortType::Out);
         PortIndex portIndexOut = c->connection().getPortIndex(PortType::Out);

         //find index of node in couterpartNode Array
         unsigned int j = 0;
         for(; j < counterpartNode.size(); ++j)
         {
            if(counterpartNode[j] == nodeIn)
               break;
         }

         unsigned int k = 0;
         for(; k < counterpartNode.size(); ++k)
         {
            if(counterpartNode[k] == nodeOut)
               break;
         }

         if(j < counterpartNode.size() && k < counterpartNode.size())
         {
            auto connection = _scene->createConnection(*createdNodes[j], portIndexIn, *createdNodes[k], portIndexOut);
            createdConnections.push_back(connection);
         }
      }
   }

   //reset selection to nodes created
   _scene->clearSelection();
   for(auto & createdNode : createdNodes)
   {
      createdNode->nodeGraphicsObject().setSelected(true);
   }

   for(auto & createdConnection : createdConnections)
   {
      createdConnection->getConnectionGraphicsObject().setSelected(true);
   }

   if(createdNodes.size() > 0)
      _scene->updateHistory();
}

void
FlowView::
copySelectedNodes()
{
   QJsonObject sceneJson;
   QJsonArray nodesJsonArray;
   std::vector<QUuid> addedIds;

   for (QGraphicsItem * item : _scene->selectedItems())
   {
      if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
      {
         Node& node = n->node();
         nodesJsonArray.append(node.save());
         addedIds.push_back(node.id());
      }
   }

   QJsonArray connectionJsonArray;
   for (QGraphicsItem * item : _scene->selectedItems())
   {
      if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item)) {
         Connection& connection = c->connection();

         if( std::find(addedIds.begin(), addedIds.end(), connection.getNode(PortType::In)->id())  != addedIds.end() &&
             std::find(addedIds.begin(), addedIds.end(), connection.getNode(PortType::Out)->id()) != addedIds.end() ) {
            QJsonObject connectionJson = connection.save();

            if (!connectionJson.isEmpty())
               connectionJsonArray.append(connectionJson);
         }
      }
   }

   sceneJson["nodes"] = nodesJsonArray;
   sceneJson["connections"] = connectionJsonArray;

   QJsonDocument document(sceneJson);
   std::string json = document.toJson().toStdString();

   QClipboard *clipboard = QApplication::clipboard();
   clipboard->setText( QString::fromStdString(json));
}

void
FlowView::
pasteSelectedNodes() {
   QClipboard *clipboard = QApplication::clipboard();
   QByteArray text = clipboard->text().toUtf8();

   QJsonObject const jsonDocument = QJsonDocument::fromJson(text).object();
   QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

   std::map<QUuid, QUuid> addedIds;

   for (QJsonValueRef element : nodesJsonArray)
   {
      auto obj = element.toObject();
      QUuid currentId = QUuid( obj["id"].toString() );
      QUuid newId = _scene->pasteNode(obj);

      addedIds.insert(std::pair<QUuid,QUuid>(currentId, newId));
   }

   QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
   for (QJsonValueRef element : connectionJsonArray)
   {
      auto obj = element.toObject();
      QUuid in = QUuid(obj["in_id"].toString());
      QUuid newIn = addedIds[in];

      QUuid out = QUuid(obj["out_id"].toString());
      QUuid newOut = addedIds[out];

      _scene->pasteConnection(obj, newIn, newOut );
   }

   _scene->updateHistory();
}

void
FlowView::
setScene(FlowScene *scene)
{
  _scene = scene;
  QGraphicsView::setScene(_scene);

  // setup actions
  delete _clearSelectionAction;
  _clearSelectionAction = new QAction(QStringLiteral("Clear Selection"), this);
  _clearSelectionAction->setShortcut(Qt::Key_Escape);
  connect(_clearSelectionAction, &QAction::triggered, _scene, &QGraphicsScene::clearSelection);
  addAction(_clearSelectionAction);

  delete _deleteSelectionAction;
  _deleteSelectionAction = new QAction(QStringLiteral("Delete Selection"), this);
  _deleteSelectionAction->setShortcut(Qt::Key_Delete);
  _deleteSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_deleteSelectionAction, &QAction::triggered, this, &FlowView::deleteSelectedNodes);
  addAction(_deleteSelectionAction);

  delete _duplicateSelectionAction;
  _duplicateSelectionAction = new QAction(QStringLiteral("Duplicate Selection"), this);
  _duplicateSelectionAction->setShortcut(QKeySequence(tr("Ctrl+D")));
  _duplicateSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_duplicateSelectionAction, &QAction::triggered, this, &FlowView::duplicateSelectedNode);
  addAction(_duplicateSelectionAction);

  delete  _copymultiplenodes;
  _copymultiplenodes = new QAction(QStringLiteral("Copy Multiple Nodes"), this);
  _copymultiplenodes->setShortcut(QKeySequence(tr("Ctrl+C")));
  _copymultiplenodes->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_copymultiplenodes, &QAction::triggered, this, &FlowView::copySelectedNodes);
  addAction(_copymultiplenodes);

  delete _pastemultiplenodes;
  _pastemultiplenodes = new QAction(QStringLiteral("Paste Multiple Nodes"), this);
  _pastemultiplenodes->setShortcut(QKeySequence(tr("Ctrl+V")));
  _pastemultiplenodes->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_pastemultiplenodes, &QAction::triggered, this, &FlowView::pasteSelectedNodes);
  addAction(_pastemultiplenodes);

  delete _undoAction;
  _undoAction = new QAction(QStringLiteral("Undo"), this);
  _undoAction->setShortcut(QKeySequence(tr("Ctrl+Z")));
  _undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_undoAction, &QAction::triggered, _scene, &FlowScene::undo);
  addAction(_undoAction);

  delete _redoAction;
  _redoAction = new QAction(QStringLiteral("Redo"), this);
  _redoAction->setShortcut(QKeySequence(tr("Ctrl+Y")));
  _redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_redoAction, &QAction::triggered, _scene, &FlowScene::redo);
  addAction(_redoAction);
}


void
FlowView::
contextMenuEvent(QContextMenuEvent *event)
{
  if (itemAt(event->pos()))
  {
    QGraphicsView::contextMenuEvent(event);
    return;
  }

  QMenu modelMenu;

  auto* modelSelectionWidget = new ModelSelectionWidget(_scene->registry(), &modelMenu);

  for (QAction* action : modelSelectionWidget->actions())
  {
    modelMenu.addAction(action);
  }

  connect(modelSelectionWidget, &ModelSelectionWidget::modelSelected, [&](QString modelName) {
    auto type = _scene->registry().create(modelName);

    if (type)
    {
      auto& node = _scene->createNode(std::move(type));

      QPoint pos = event->pos();

      QPointF posView = this->mapToScene(pos);

      node.nodeGraphicsObject().setPos(posView);
      _scene->updateHistory();

      _scene->nodePlaced(node);
    }
    else
    {
      qDebug() << "Model not found";
    }

    modelMenu.close();
  });

  modelMenu.exec(event->globalPos());
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
      _scene->removeNode(n->node());
  }

  _scene->updateHistory();
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

      // TODO #133 check why
      if(right - left > 100)
         return;

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


FlowScene *
FlowView::
scene()
{
  return _scene;
}
