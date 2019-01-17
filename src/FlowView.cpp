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

using QtNodes::FlowView;
using QtNodes::FlowScene;

FlowView::
FlowView(QWidget *parent)
  : QGraphicsView(parent)
  , _clearSelectionAction(Q_NULLPTR)
  , _deleteSelectionAction(Q_NULLPTR)
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
  
  setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
  
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
 
  
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


void
FlowView::setScene(FlowScene *scene)
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
  
  _duplicateSelectionAction = new QAction(QStringLiteral("Duplicate Selection"), this);
  _duplicateSelectionAction->setShortcut(QKeySequence(tr("Ctrl+D")));
  _duplicateSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_duplicateSelectionAction, &QAction::triggered, this, &FlowView::duplicateSelectedNode);
  addAction(_duplicateSelectionAction);

  _copymultiplenodes = new QAction(QStringLiteral("Copy Multiple Nodes"), this);
  _copymultiplenodes->setShortcut(QKeySequence(tr("Ctrl+C")));
  _copymultiplenodes->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_copymultiplenodes, &QAction::triggered, this, &FlowView::copySelectedNodes);
  addAction(_copymultiplenodes);
  
  _pastemultiplenodes = new QAction(QStringLiteral("Paste Multiple Nodes"), this);
  _pastemultiplenodes->setShortcut(QKeySequence(tr("Ctrl+V")));
  _pastemultiplenodes->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_pastemultiplenodes, &QAction::triggered, this, &FlowView::pasteSelectedNodes);
  addAction(_pastemultiplenodes);
  
  _undoAction = new QAction(QStringLiteral("Undo"), this);
  _undoAction->setShortcut(QKeySequence(tr("Ctrl+Z")));
  _undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_undoAction, &QAction::triggered, _scene, &FlowScene::Undo);
  addAction(_undoAction);
  
  _redoAction = new QAction(QStringLiteral("Redo"), this);
  _redoAction->setShortcut(QKeySequence(tr("Ctrl+Y")));
  _redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(_redoAction, &QAction::triggered, _scene, &FlowScene::Redo);
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

  connect(treeView, &QTreeWidget::itemActivated, [&](QTreeWidgetItem *item, int)
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

      QPoint pos = event->pos();

      QPointF posView = this->mapToScene(pos);

      node.nodeGraphicsObject().setPos(posView);
	  
	  _scene->UpdateHistory();
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
        if (modelName.contains(text, Qt::CaseInsensitive))
        {
          child->setHidden(false);
        }
        else
        {
          child->setHidden(true);
        }
      }
    }
  });

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

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
  //std::cout << "deleteSelectedNodes" << std::endl; 
  // delete the nodes, this will delete many of the connections
  std::vector<Node*> nodeItems = _scene->selectedNodes();
  for(int i = 0; i < nodeItems.size(); i++)
  {
	Node* item = nodeItems[i]; 
	if(item)
	{
		_scene->removeNode(*item);
	}
  }

  for (QGraphicsItem * item : _scene->selectedItems())
  {
	if(item)
	{
		if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
			_scene->deleteConnection(c->connection());
	}
  }
  
  _scene->UpdateHistory(); 
}

void FlowView::copySelectedNodes() {
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

  QClipboard *p_Clipboard = QApplication::clipboard();
  p_Clipboard->setText( QString::fromStdString(json));
}

void FlowView::pasteSelectedNodes() {
    QClipboard *p_Clipboard = QApplication::clipboard();  
    QByteArray text = p_Clipboard->text().toUtf8();

    QJsonObject const jsonDocument = QJsonDocument::fromJson(text).object();
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    std::map<QUuid, QUuid> addedIds;

    for (int i = 0; i < nodesJsonArray.size(); ++i)
    {
      QUuid currentId = QUuid( nodesJsonArray[i].toObject()["id"].toString() );
      QUuid newId = _scene->pasteNode(nodesJsonArray[i].toObject());

      addedIds.insert(std::pair<QUuid,QUuid>(currentId, newId));
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
    for (int i = 0; i < connectionJsonArray.size(); ++i)
    {
      QUuid in = QUuid(connectionJsonArray[i].toObject()["in_id"].toString());
      QUuid newIn = addedIds[in];
      
      QUuid out = QUuid(connectionJsonArray[i].toObject()["out_id"].toString());
      QUuid newOut = addedIds[out];

      _scene->pasteConnection(connectionJsonArray[i].toObject(), newIn, newOut );
    }
    
    _scene->UpdateHistory();
}

void FlowView::duplicateSelectedNode()
{
	//Get Bounds of all the selected items 
	float minx = 10000000000;
	float miny = 10000000000;
	float maxx = -1000000000;
	float maxy = -1000000000;
	for (QGraphicsItem * item : _scene->selectedItems())
	{
		if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
		{
			QPointF pos = n->pos();
			if(pos.x() < minx) minx = pos.x();
			if(pos.y() < miny) miny = pos.y();
			if(pos.x() > maxx) maxx = pos.x();
			if(pos.y() > maxy) maxy = pos.y();
		}
	}
	//compute centroid
	float centroidX = (maxx - minx) / 2.0 + minx;
	float centroidY = (maxy - miny) / 2.0 + miny;
	QPointF centroid(centroidX, centroidY);
	
	//create nodes
	std::vector<Node*> createdNodes;
	std::vector<Node*> couterpartNode; 
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
			  couterpartNode.push_back(&(n->node()));
			  
			  QPoint viewPointMouse = this->mapFromGlobal(QCursor::pos());
			  QPointF posViewMouse = this->mapToScene(viewPointMouse);

			  QPointF pos = posViewMouse + (n->pos() - centroid);
			  
			  node.nodeGraphicsObject().setPos(pos);
			}
			else
			{
			  qDebug() << "Model not found";
			}
		}
	}
	
	//create connections 
	std::vector<std::shared_ptr<Connection> > createdConnections; 
	for (QGraphicsItem * item : _scene->selectedItems())
	{
	    if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
		{
			//if(c->connection().connectionState().)
			
			Node* nodeIn = c->connection().getNode(PortType::In); 
			PortIndex portIndexIn = c->connection().getPortIndex(PortType::In); 
			Node* nodeOut = c->connection().getNode(PortType::Out); 
			PortIndex portIndexOut = c->connection().getPortIndex(PortType::Out); 
			
			//find index of node in couterpartNode Array
			int j = -1;
			for(j = 0; j < couterpartNode.size(); j++)
			{
				if(couterpartNode[j] == nodeIn)
					break;
			}
			
			int k = -1;
			for(k = 0; k < couterpartNode.size(); k++)
			{
				if(couterpartNode[k] == nodeOut)
					break;
			}
			
			if(j >=0 && k>=0 && j < couterpartNode.size() && k < couterpartNode.size())
			{
				auto connection = _scene->createConnection(*createdNodes[j], portIndexIn, *createdNodes[k], portIndexOut);
				auto& connectionRef = connection; 
				createdConnections.push_back(connection);
			}
		}
	}

	
	//reset selection to nodes created
	_scene->clearSelection();
	for(int i = 0; i < createdNodes.size(); i++)
	{
		createdNodes[i]->nodeGraphicsObject().setSelected(true);
	}
	for(int i = 0; i < createdConnections.size(); i++)
	{
		createdConnections[i]->getConnectionGraphicsObject().setSelected(true);
	}
	
	
	if(createdNodes.size() > 0)
		_scene->UpdateHistory();
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
