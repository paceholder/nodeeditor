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
#include "Group.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "StyleCollection.hpp"

#include "Connection.hpp"
#include "NodeConnectionInteraction.hpp"

using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::Connection;
using QtNodes::NodeConnectionInteraction;
using QtNodes::NodeGraphicsObject;

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
FlowView::addAnchor(int index) 
{
  qreal x1, y1, x2, y2;
  sceneRect().getCoords(&x1, &y1, &x2, &y2);

  Anchor a;
  a.position = QPointF((x2 + x1) * 0.5, (y1 + y2) * 0.5);
  a.scale = 10;
  
  _scene->anchors[index] = a;
}

void 
FlowView::goToAnchor(int index) 
{
  qreal x1, y1, x2, y2;
  sceneRect().getCoords(&x1, &y1, &x2, &y2);
  QPointF currentPosition = QPointF((x2 + x1) * 0.5, (y1 + y2) * 0.5);

  QPointF difference = _scene->anchors[index].position - currentPosition;
  
  setSceneRect(sceneRect().translated(difference.x(), difference.y()));    
}

void FlowView::goToNode(NodeGraphicsObject *node)
{
  qreal x1, y1, x2, y2;
  sceneRect().getCoords(&x1, &y1, &x2, &y2);
  QPointF currentPosition = QPointF((x2 + x1) * 0.5, (y1 + y2) * 0.5);

  QPointF difference = node->pos() - currentPosition;
  
  setSceneRect(sceneRect().translated(difference.x(), difference.y()));  
     
    
  float scaleX = 1.2f / transform().m11();
  float scaleY = 1.2f / transform().m22();
  scale(scaleX, scaleY);
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

  for(int i=0; i<10; i++) {
    QAction* _addAnchor = new QAction(QStringLiteral("Add Anchor"), this);
    QString sequenceString = QString("Ctrl+") + QString::number(i);
    _addAnchor->setShortcut(QKeySequence(sequenceString));
    _addAnchor->setShortcutContext(Qt::WidgetWithChildrenShortcut	);
    connect(_addAnchor, &QAction::triggered, _scene, [this, i]() {
      addAnchor(i);
    });
    addAction(_addAnchor);
    anchorActions.push_back(_addAnchor);

    QAction* _goToAnchor = new QAction(QStringLiteral("Go to Anchor"), this);
    _goToAnchor->setShortcut(QKeySequence(tr(std::to_string(i).c_str())));
    _goToAnchor->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(_goToAnchor, &QAction::triggered, _scene, [this, i]() {
      goToAnchor(i);
    });
    addAction(_goToAnchor);
    anchorActions.push_back(_goToAnchor);
  }
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

  //Add templates category
  auto templatesCategory = new QTreeWidgetItem(treeView);
  templatesCategory->setText(0, "Templates");
  templatesCategory->setData(0, Qt::UserRole, "Templates");
  topLevelItems["Templates"] = templatesCategory;

  for (auto const &assoc : _scene->registry().registeredModelsCategoryAssociation())
  {
    auto parent = topLevelItems[assoc.second];
    auto item   = new QTreeWidgetItem(parent);
    item->setText(0, assoc.first);
    item->setData(0, Qt::UserRole, assoc.first);
  }
  
  for (auto const &assoc : _scene->registry().RegisteredTemplates())
  {
    QString name = assoc.first;
    auto item   = new QTreeWidgetItem(templatesCategory);
    item->setText(0, name);
    item->setData(0, Qt::UserRole, name);
  }

  //Add templates category
  // auto GroupsCategory = new QTreeWidgetItem(treeView);
  // GroupsCategory->setText(0, "Groups");
  // GroupsCategory->setData(0, Qt::UserRole, "Groups");
  // topLevelItems["Groups"] = GroupsCategory;

  auto groupItem   = new QTreeWidgetItem(treeView);
  groupItem->setText(0, "Group");
  groupItem->setData(0, Qt::UserRole, "Group");
  topLevelItems["Group"] = groupItem;
  

  treeView->expandAll();


  connect(treeView, &QTreeWidget::itemActivated, [&](QTreeWidgetItem *item, int)
  {
    QString modelName = item->data(0, Qt::UserRole).toString();

    if (modelName == skipText)
    {
      return;
    }

    if(modelName == "Group")
    {
      Group& group = _scene->createGroup();
      QPoint pos = event->pos();
      QPointF posView = this->mapToScene(pos);
      group.groupGraphicsObject().setPos(posView);
      modelMenu.close();
      return;
    }

	if (item->parent() != nullptr)
	{
		QString parent = item->parent()->data(0, Qt::UserRole).toString();
		if(parent == "Templates")
		{
		  DataModelRegistry::RegisteredTemplatesMap map = _scene->registry().RegisteredTemplates();
		  QString fileName = map[modelName];

		  QFile file;
		  file.setFileName(fileName);
		  file.open(QIODevice::ReadOnly | QIODevice::Text);
		  QString val = file.readAll();
		  file.close();
		  qWarning() << val;
		  QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
		  QJsonObject sett2 = d.object();
		  jsonToSceneMousePos(sett2);

			modelMenu.close();
		}
	}

    auto type = _scene->registry().create(modelName);

    if (type)
    {
      Node& node = _scene->createNode(std::move(type));
      QPoint pos = event->pos();
      QPointF posView = this->mapToScene(pos);
      node.nodeGraphicsObject().setPos(posView);

      QUuid id = node.id();
      _scene->AddAction(UndoRedoAction(
        [this, id](void *ptr)
        {
          _scene->removeNodeWithID(id);
          return 0;
        },
        [this, pos, modelName, id](void *ptr)
        {
          auto type = _scene->registry().create(modelName);
          auto& node = _scene->createNodeWithID(std::move(type), id);
          QPointF posView = this->mapToScene(pos);
          node.nodeGraphicsObject().setPos(posView);    
          return 0;
        },
        "Created Node " + node.nodeDataModel()->name().toStdString()
      ));
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
      bool shouldHideCategory = true;
      for (int i = 0; i < topLvlItem->childCount(); ++i)
      {
        auto child = topLvlItem->child(i);
        auto modelName = child->data(0, Qt::UserRole).toString();
        if (modelName.contains(text, Qt::CaseInsensitive))
        {
          shouldHideCategory = false;
          child->setHidden(false);
        }
        else
        {
          child->setHidden(true);
        }
      }
      auto catName = topLvlItem->data(0, Qt::UserRole).toString();
      if(catName.contains(text, Qt::CaseInsensitive))
      {
        shouldHideCategory=false;
      }

      topLvlItem->setHidden(shouldHideCategory);
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
	QJsonObject sceneJson = selectionToJson(true);

  std::vector<QUuid> nodeIds;
  std::vector<QUuid> connectionsIds;
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if(item)
    {
      if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
      {
          _scene->deleteConnection(c->connection());      
      }
    }
  }
  for (QGraphicsItem * item : _scene->selectedItems())
  {
    if(item)
    {
      if (auto c = qgraphicsitem_cast<GroupGraphicsObject*>(item))
      {
        _scene->removeGroup(c->group());      
      }
      else if (auto c = qgraphicsitem_cast<NodeGraphicsObject*>(item))
      {
          _scene->removeNode(c->node());
      }
    }
  }
  
  
  _scene->AddAction(UndoRedoAction(
    //Undo action
    [this, sceneJson](void *ptr) 
    {
      jsonToScene(sceneJson);
      return 0;
    },
    //Redo action
    [this, sceneJson](void *ptr)
    {
      deleteJsonElements(sceneJson);
      return 0;
    },
    //Name
    "Deletes nodes"
  ));
}

void FlowView::deleteJsonElements(const QJsonObject &jsonDocument)
{
  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();
  for (int i = 0; i < nodesJsonArray.size(); ++i)
  {
    QJsonObject nodeJson = nodesJsonArray[i].toObject();
    QUuid id = QUuid( nodeJson["id"].toString() );
    _scene->removeNodeWithID(id);
  }

}

void FlowView::copySelectedNodes() {
  QJsonObject sceneJson = selectionToJson();

  QJsonDocument document(sceneJson);
  std::string json = document.toJson().toStdString();

  QClipboard *p_Clipboard = QApplication::clipboard();
  p_Clipboard->setText( QString::fromStdString(json));
}

void FlowView::jsonToScene(QJsonObject jsonDocument)
{
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();



    for (int i = 0; i < nodesJsonArray.size(); ++i)
    {
      _scene->restoreNode(nodesJsonArray[i].toObject(), true);
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
    for (int i = 0; i < connectionJsonArray.size(); ++i)
    {
      QUuid in = QUuid(connectionJsonArray[i].toObject()["in_id"].toString());      
      QUuid out = QUuid(connectionJsonArray[i].toObject()["out_id"].toString());

      _scene->pasteConnection(connectionJsonArray[i].toObject(), in, out );
    }
    
    QJsonArray groupsJsonArray = jsonDocument["groups"].toArray();
    for (int i = 0; i < groupsJsonArray.size(); ++i)
    {
      _scene->restoreGroup(groupsJsonArray[i].toObject());
    }
}


void FlowView::jsonToSceneMousePos(QJsonObject jsonDocument)
{
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    std::map<QUuid, QUuid> addedIds;

    //Get Bounds of all the selected items 
    float minx = 10000000000;
    float miny = 10000000000;
    float maxx = -1000000000;
    float maxy = -1000000000;
    for (int i = 0; i < nodesJsonArray.size(); ++i)
    {
      QJsonObject nodeJsonObject = nodesJsonArray[i].toObject();
      QJsonObject positionJson = nodeJsonObject["position"].toObject();
      QPointF pos(positionJson["x"].toDouble(), positionJson["y"].toDouble());

      if(pos.x() < minx) minx = pos.x();
      if(pos.y() < miny) miny = pos.y();
      if(pos.x() > maxx) maxx = pos.x();
      if(pos.y() > maxy) maxy = pos.y();     
    }
    
    float centroidX = (maxx - minx) / 2.0 + minx;
    float centroidY = (maxy - miny) / 2.0 + miny;
    QPointF centroid(centroidX, centroidY);

    QPoint viewPointMouse = this->mapFromGlobal(QCursor::pos());
    QPointF posViewMouse = this->mapToScene(viewPointMouse);      
  



    for (int i = 0; i < nodesJsonArray.size(); ++i)
    {
      QJsonObject nodeJson = nodesJsonArray[i].toObject();
      QUuid currentId = QUuid( nodeJson["id"].toString() );
      QUuid newId = _scene->pasteNode(nodesJsonArray[i].toObject(), centroid, posViewMouse);

      addedIds.insert(std::pair<QUuid,QUuid>(currentId, newId));
      nodesJsonArray[i] = _scene->nodes()[newId]->save();
    }
    jsonDocument["nodes"] = nodesJsonArray;

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
    for (int i = 0; i < connectionJsonArray.size(); ++i)
    {
      QJsonObject connectionJson = connectionJsonArray[i].toObject();

      QUuid in = QUuid(connectionJson["in_id"].toString());
      QUuid newIn = addedIds[in];
      
      QUuid out = QUuid(connectionJson["out_id"].toString());
      QUuid newOut = addedIds[out];

      _scene->pasteConnection(connectionJson, newIn, newOut );

      connectionJson["in_id"] = newIn.toString();
      connectionJson["out_id"] = newOut.toString();
      connectionJsonArray[i] = connectionJson;
    }
    jsonDocument["connections"] =connectionJsonArray;
    
    QJsonArray groupsJsonArray = jsonDocument["groups"].toArray();
    for (int i = 0; i < groupsJsonArray.size(); ++i)
    {
      _scene->pasteGroup(groupsJsonArray[i].toObject(), centroid, posViewMouse);
    }
  

    _scene->AddAction(UndoRedoAction(
        [this, addedIds](void *ptr)
        {
          //Delete all the created nodes (and their connections)
          for(auto &id: addedIds)
          {
            _scene->removeNodeWithID(id.second);
          }
          return 0;
        },
        [this, jsonDocument](void *ptr)
        {       
          jsonToScene(jsonDocument); //jsonDocument has now been updated with new IDs and new positions
          return 0;
        },
        "Created Node "
      ));

}

QJsonObject FlowView::selectionToJson(bool includePartialConnections)
{
  QJsonObject sceneJson;
  QJsonArray nodesJsonArray;
  QJsonArray connectionJsonArray;
  std::vector<QUuid> addedIds;
  
  for (QGraphicsItem * item : _scene->selectedItems())
	{
		if (auto n = qgraphicsitem_cast<NodeGraphicsObject*>(item))
		{
        Node& node = n->node();
        nodesJsonArray.append(node.save());
        addedIds.push_back(node.id());

        if(includePartialConnections)
        {
          std::vector<Connection*> allConnections = node.nodeState().allConnections();
          for(int i=0; i<allConnections.size(); i++)
          {
            QJsonObject connectionJson = allConnections[i]->save();
            if (!connectionJson.isEmpty())
              connectionJsonArray.append(connectionJson);
          }
        }
    }
  }

  for (QGraphicsItem * item : _scene->selectedItems())
	{
    if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject*>(item)) {
        Connection& connection = c->connection();

        if((std::find(addedIds.begin(), addedIds.end(), connection.getNode(PortType::In)->id())  != addedIds.end() && 
            std::find(addedIds.begin(), addedIds.end(), connection.getNode(PortType::Out)->id()) != addedIds.end()) || includePartialConnections) {
          QJsonObject connectionJson = connection.save();
          
          if (!connectionJson.isEmpty())
            connectionJsonArray.append(connectionJson);
        }
    }
  }
  
  QJsonArray groupJsonArray;
  for (QGraphicsItem * item : _scene->selectedItems())
	{
    if (auto c = qgraphicsitem_cast<GroupGraphicsObject*>(item)) {
        Group& group = c->group();

        //If collapsed
        if(group.groupGraphicsObject().isCollapsed())
        {
          //Add all child items to nodes array
          for(int i=0; i<group.groupGraphicsObject().childItems().size(); i++)
          {
            NodeGraphicsObject* ngo = dynamic_cast<NodeGraphicsObject*>(group.groupGraphicsObject().childItems()[i]);
            if(ngo!=nullptr)
            {
              Node& node = ngo->node();
              nodesJsonArray.append(node.save());
            }
          }

          //Add all connections to nodes array
          for(int i=0; i<group.groupGraphicsObject().inOutConnections.size(); i++)
          {
            for(int j=0; j<group.groupGraphicsObject().inOutConnections[i].size(); j++)
            {
              QJsonObject connectionJson = group.groupGraphicsObject().inOutConnections[i][j]->save();
              if (!connectionJson.isEmpty())
                connectionJsonArray.append(connectionJson);              
            }
          }
        }

        QJsonObject groupJson = group.save();
        if (!groupJson.isEmpty())
          groupJsonArray.append(groupJson);
    }
  }

  sceneJson["nodes"] = nodesJsonArray;
  sceneJson["connections"] = connectionJsonArray;
  sceneJson["groups"] = groupJsonArray;

  return sceneJson;
}

void FlowView::pasteSelectedNodes() {
    QClipboard *p_Clipboard = QApplication::clipboard();  
    QByteArray text = p_Clipboard->text().toUtf8();

    QJsonObject const jsonDocument = QJsonDocument::fromJson(text).object();
    jsonToSceneMousePos(jsonDocument);
    

}



void FlowView::duplicateSelectedNode()
{
  QJsonObject selectionJson = selectionToJson();
  jsonToSceneMousePos(selectionJson);
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
