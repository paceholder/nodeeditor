#include "GroupGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "Connection.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "Group.hpp"
#include "NodeDataModel.hpp"
#include "NodeConnectionInteraction.hpp"

#include "StyleCollection.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::Group;
using QtNodes::FlowScene;
using QtNodes::Connection;
using QtNodes::PortType;

GroupGraphicsObject::
GroupGraphicsObject(FlowScene &scene, Group& group)
  : _scene(scene)
  , _group(group)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  setAcceptHoverEvents(true);
  setAcceptTouchEvents(true);

  setZValue(-2);

  r = g = b = 135;

  _proxyWidget = new QGraphicsProxyWidget(this);
  _collapseButton = new QGraphicsProxyWidget(this);
  
  QString name = group.GetName();
  nameLineEdit = new QLineEdit(name);
  nameLineEdit->setAlignment(Qt::AlignHCenter);
  nameLineEdit->setStyleSheet("\
  QLineEdit {\
    border: 0px solid gray;\
    border-radius: 0px;\
    padding: 0 0px;\
    background: rgb(135, 135, 135);\
    selection-background-color: darkgray;\
    color: rgb(220, 220, 220);\
    font-size: 30px;\
  }\
  QLineEdit:hover {\
    background: rgb(90, 90, 90);\
  }\
  ");

  connect(nameLineEdit, &QLineEdit::textChanged, this, [&group](const QString &text) {
    group.SetName(text);
  });

  _proxyWidget->setWidget(nameLineEdit);
  _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));

  collapseButtonWidget = new QPushButton("X");
  collapseButtonWidget->setCheckable(true);
  _collapseButton->setWidget(collapseButtonWidget);
  _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));

  connect(collapseButtonWidget, &QPushButton::clicked, this, [this](int state){
    Collapse();
  });
}

void 
GroupGraphicsObject::
Collapse()
{
	unusedConnections.clear();

	////1. Identify all the nodes that have external inputs
	inOutLabels[(int)PortType::In].clear();
	inOutConnections[(int)PortType::In].clear();
	inOutNodes[(int)PortType::In].clear();
	inOutPorts[(int)PortType::In].clear();
	//for all the nodes in the group
	for (int i = 0; i < childItems().size(); i++)
	{
		NodeGraphicsObject* nodeGO = qgraphicsitem_cast<NodeGraphicsObject*>(childItems()[i]);
		if (nodeGO != nullptr)
		{
			Node &node = nodeGO->node();
			int numEntries = node.nodeState().getEntries(PortType::In).size();

			//For each input of the current node
			for (int j = 0; j < numEntries; j++)
			{
				NodeState::ConnectionPtrSet connections = node.nodeState().connections(PortType::In, j);
				//Get the connection
				for (std::pair<QUuid, Connection*> connectionPair : connections)
				{
					Connection *connection = connectionPair.second;
					Node *sourceNode = connection->getNode(PortType::Out);
					Node *destNode = connection->getNode(PortType::In);
					NodeGraphicsObject &sourceNodeGO = sourceNode->nodeGraphicsObject();

					//Check if the the input is inside the group
					if (childItems().indexOf(&sourceNodeGO) == -1) { //if it's not inside
						inOutLabels[(int)PortType::In].push_back(
							node.nodeDataModel()->name() + "." + node.nodeDataModel()->portCaption(PortType::In, j)
						);
						inOutConnections[(int)PortType::In].push_back(connection);
						inOutNodes[(int)PortType::In].push_back(destNode);
						inOutPorts[(int)PortType::In].push_back(j);
					}
					else { //it's inside
						unusedConnections.push_back(connection);
					}
				}
			}
		}
	}

	////2. Identify all the nodes that have external outputs
	inOutLabels[(int)PortType::Out].clear();
	inOutConnections[(int)PortType::Out].clear();
	inOutNodes[(int)PortType::Out].clear();
	inOutPorts[(int)PortType::Out].clear();
	//for all the nodes in the group
	for (int i = 0; i < childItems().size(); i++)
	{
		NodeGraphicsObject* nodeGO = qgraphicsitem_cast<NodeGraphicsObject*>(childItems()[i]);
		if (nodeGO != nullptr)
		{
			Node &node = nodeGO->node();
			int numOutputs = node.nodeState().getEntries(PortType::Out).size();
			for (int j = 0; j < numOutputs; j++)
			{
				NodeState::ConnectionPtrSet connections = node.nodeState().connections(PortType::Out, j);
				for (std::pair<QUuid, Connection*> connectionPair : connections)
				{
					Connection *connection = connectionPair.second;
					Node *destNode = connection->getNode(PortType::In);
					NodeGraphicsObject &destNodeGO = destNode->nodeGraphicsObject();

					if (childItems().indexOf(&destNodeGO) == -1) {
						inOutLabels[(int)PortType::Out].push_back(
							node.nodeDataModel()->name() + "." + node.nodeDataModel()->portCaption(PortType::Out, j)
						);
						inOutConnections[(int)PortType::Out].push_back(connection);
						inOutNodes[(int)PortType::Out].push_back(destNode);
						inOutPorts[(int)PortType::Out].push_back(j);
					}
					else {
						unusedConnections.push_back(connection);
					}
				}
			}
		}
	}


  if(!collapsed)
  {
    int numInOut = std::max(inOutLabels[(int)PortType::In].size(), inOutLabels[(int)PortType::Out].size());

    savedSizeX = sizeX;
    savedSizeY = sizeY;

    //Do resize
    sizeX = 500;
    sizeY = numInOut * spacing;
    _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
    _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));


    //Sets the inside nodes invisible
    for(int i=0; i<childItems().size(); i++)
    {
      NodeGraphicsObject* ngo = dynamic_cast<NodeGraphicsObject*>(childItems()[i]);
      if(ngo!=nullptr)
      {
        childItems()[i]->setVisible(false);
      }
    }

    //Sets the inside connections invisible
    for(int i=0; i<unusedConnections.size(); i++)
    {
      unusedConnections[i]->getConnectionGraphicsObject().setVisible(false);
    }
    
    //Change the input connection positions
    for(int i=0;i<inOutConnections[(int)PortType::In].size(); i++)
    {
      QPointF position = portScenePosition(i, PortType::In);
      QPointF connectionPos = inOutConnections[(int)PortType::In][i]->getConnectionGraphicsObject().sceneTransform().inverted().map(position);    
      inOutConnections[(int)PortType::In][i]->connectionGeometry().setEndPoint(PortType::In, connectionPos);

      
      Group &thisGroup = group();
      inOutConnections[(int)PortType::In][i]->setGroup(&thisGroup, PortType::In, i);
    }

    //Change the output connection positions
    for(int i=0;i<inOutConnections[(int)PortType::Out].size(); i++)
    {
      QPointF position = portScenePosition(i, PortType::Out);
      QPointF connectionPos = inOutConnections[(int)PortType::Out][i]->getConnectionGraphicsObject().sceneTransform().inverted().map(position);    
      inOutConnections[(int)PortType::Out][i]->connectionGeometry().setEndPoint(PortType::Out,  connectionPos);

      Group &thisGroup = group();
      inOutConnections[(int)PortType::Out][i]->setGroup(&thisGroup, PortType::Out, i);
    }

    collapsed=true;
  }
  else
  {
    sizeX = savedSizeX;
    sizeY = savedSizeY;
    
    _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
    _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));

    //Sets the inside nodes invisible
    for(int i=0; i<childItems().size(); i++)
    {
      NodeGraphicsObject* ngo = dynamic_cast<NodeGraphicsObject*>(childItems()[i]);
      if(ngo!=nullptr)
      {
        childItems()[i]->setVisible(true);
      }
    }    

    for(int i=0; i<unusedConnections.size(); i++)
    {
      unusedConnections[i]->getConnectionGraphicsObject().setVisible(true);
    }

    for(int i=0; i<inOutConnections[(int)PortType::In].size(); i++)
    {
      inOutConnections[(int)PortType::In][i]->setGroup(nullptr, PortType::In, 0);
    }

    for(int i=0; i<inOutConnections[(int)PortType::Out].size(); i++)
    {
      inOutConnections[(int)PortType::Out][i]->setGroup(nullptr, PortType::Out, 0);
    }

    collapsed=false;
    unusedConnections.clear();;
    inOutLabels[(int)PortType::In].clear();
    inOutLabels[(int)PortType::Out].clear();
    inOutConnections[(int)PortType::In].clear();
    inOutConnections[(int)PortType::Out].clear();
    moveConnections();
  }
}

QPointF GroupGraphicsObject::portScenePosition(int i, PortType type) const
{
  
  if(type == PortType::In)
  {
    return scenePos() +QPointF(inputSize, topPadding + i * spacing);
  }
  else
  {
    return scenePos() +QPointF(sizeX - inputSize, topPadding + i * spacing);
  }
}

QPointF GroupGraphicsObject::portPosition(int i, PortType type) const
{
  
  if(type == PortType::In)
  {
    return QPointF(inputSize, topPadding + i * spacing);
  }
  else
  {
    return QPointF(sizeX - inputSize, topPadding + i * spacing);
  }
}

int GroupGraphicsObject::checkHitScenePoint(PortType portType,
                   QPointF const scenePoint,
                   QTransform sceneTransform) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  PortIndex result = INVALID;

  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

  size_t const nItems = inOutLabels[(int)portType].size();

  for (size_t i = 0; i < nItems; ++i)
  {
    auto pp = portScenePosition(i, portType);

    QPointF p = pp - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));
    
    if (distance < tolerance)
    {
      result = PortIndex(i);
      break;
    }
  }

  return result;
}


GroupGraphicsObject::
~GroupGraphicsObject()
{
  _scene.removeItem(this);
}


Group&
GroupGraphicsObject::
group() {
  return _group;
}

Group const&
GroupGraphicsObject::
group() const {
  return _group;
}


QRectF
GroupGraphicsObject::
boundingRect() const
{
    return QRectF(0, 0, sizeX, sizeY);
}


void
GroupGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}

void
GroupGraphicsObject::
moveConnections() const {
  for(int i=0; i<childItems().size(); i++) {
    NodeGraphicsObject* ngo = dynamic_cast<NodeGraphicsObject*>(childItems()[i]);
    if (ngo) {
      ngo->moveConnections();
    }

    GroupGraphicsObject* ggo = dynamic_cast<GroupGraphicsObject*>(childItems()[i]);
    if (ggo) {
      ggo->moveConnections();
    }
  }
}



void
GroupGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);
  QRect rect(0, 0, sizeX, sizeY);
  auto color = QColor(r, g, b, 255);
  painter->drawRect(rect);
  painter->fillRect(rect, QBrush(color));
  
  QFontMetrics const & metrics =
  painter->fontMetrics();

  auto drawPoints =
  [&](PortType portType)
  {
    size_t n = inOutLabels[(int)portType].size();
    for (size_t i = 0; i < n; ++i)
    {
      QPointF p = portPosition(i, portType);
      
      painter->setPen(QColor(255, 255, 255, 255));

      QString s = inOutLabels[(int)portType][i];
      
      auto rect = metrics.boundingRect(s);

      p.setY(p.y() + rect.height() / 4.0);
      switch (portType)
      {
        case PortType::In:
          p.setX(inputSize + 5.0);
          break;

        case PortType::Out:
          p.setX(sizeX - 5.0 - rect.width() - inputSize);
          break;

        default:
          break;
      }

      painter->drawText(p, s);
    }
  };

  drawPoints(PortType::Out);
  drawPoints(PortType::In);
}


QVariant
GroupGraphicsObject::
itemChange(GraphicsItemChange change, const QVariant &value)
{
  return QGraphicsItem::itemChange(change, value);
}


void
GroupGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
  {
    event->ignore();
    return;
  }  

  // deselect all other items after this one is selected
  if (!isSelected() &&
      !(event->modifiers() & Qt::ControlModifier))
  {
    _scene.clearSelection();
  }

  auto mousePos = event->pos();

  if (abs(mousePos.x() - sizeX) < 20 && abs(mousePos.y() - sizeY) < 20)
  {
    isResizingXY=true;
  } else  if (abs(mousePos.x() - sizeX) < 20)
  {
    isResizingX=true;
  } else  if (abs(mousePos.y() - sizeY) < 20) {
    isResizingY=true;
  }

  auto clickPort =
    [&](PortType portToCheck)
    {
      
      // TODO do not pass sceneTransform
      int portIndex = checkHitScenePoint(portToCheck,
                                                      event->scenePos(),
                                                      sceneTransform());
      if (portIndex != INVALID)
      {
        // start dragging existing connection
        if (portToCheck == PortType::In)
        {
          auto con = inOutConnections[(int)PortType::In][portIndex];
          Node *currentNode = inOutConnections[(int)PortType::In][portIndex]->getNode(PortType::In);
          Node &currentNodeRef = *currentNode;
          NodeConnectionInteraction interaction(currentNodeRef, *con, _scene);
          interaction.disconnect(portToCheck);
        }
        else // initialize new Connection
        {
          //Get the node from which the connection is created
          Node *currentNode = inOutConnections[(int)PortType::Out][portIndex]->getNode(PortType::Out);
          int nodePortIndex = inOutConnections[(int)PortType::Out][portIndex]->getPortIndex(PortType::Out);
          Node &currentNodeRef = *currentNode;
          
          //Create the group connection
          auto connection = _scene.createConnection(portToCheck,
                                                    currentNodeRef,   
                                                    nodePortIndex);

          //Set connection position
          QPointF position = portScenePosition(portIndex, PortType::Out);
          QPointF connectionPos = connection->getConnectionGraphicsObject().sceneTransform().inverted().map(position);    
          connection->connectionGeometry().setEndPoint(PortType::In,  connectionPos);
          connection->connectionGeometry().setEndPoint(PortType::Out, connectionPos);

          //Set the group to the connection
          Group &thisGroup = group();
          connection->setGroup(&thisGroup, PortType::Out, portIndex);
          

          //Add to the connections
          inOutConnections[(int)PortType::Out].push_back(connection.get());

          //Set the connection to the node
          currentNodeRef.nodeState().setConnection(portToCheck,
                                          nodePortIndex,
                                          *connection);

          
          connection->getConnectionGraphicsObject().grabMouse();
          connection->getConnectionGraphicsObject().move();
        }
      }
    };

  clickPort(PortType::In);
  clickPort(PortType::Out);
}


void
GroupGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(isResizingX) {
    int diff = event->pos().x() - event->lastPos().x();
    prepareGeometryChange();
    sizeX += diff;
    update();
    _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
    _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));
    event->accept();
  }
  else if(isResizingY) {
    int diff = event->pos().y() - event->lastPos().y();
    prepareGeometryChange();
    sizeY += diff;
    update();
    _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
    _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));
    event->accept();
  } else if(isResizingXY) {
    auto diff = event->pos() - event->lastPos();
    prepareGeometryChange();
    sizeX += diff.x();
    sizeY += diff.y();
    update();
    _proxyWidget->setPos(QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
    _collapseButton->setPos(QPointF(sizeX - _collapseButton->size().width(), 0));
    event->accept();    
  } else {
    _scene.groupMoved(_group, pos());
    QGraphicsObject::mouseMoveEvent(event);
    moveConnections();
  }
}


void
GroupGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsObject::mouseReleaseEvent(event);
  _scene.groupMoveFinished(_group, pos());

  isResizingX=false;
  isResizingY=false;
  isResizingXY=false;
}


void
GroupGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  auto mousePos    = event->pos();
  setCursor(QCursor());  
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto mousePos    = event->pos();

  if (abs(mousePos.x() - sizeX) < 20 && abs(mousePos.y() - sizeY) < 20)
  {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else  if (abs(mousePos.x() - sizeX) < 20)
  {
    setCursor(QCursor(Qt::SizeHorCursor));
  } else  if (abs(mousePos.y() - sizeY) < 20) {
    setCursor(QCursor(Qt::SizeVerCursor));
  } else
  {
    setCursor(QCursor());
  }

  event->accept();
}


void
GroupGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  _scene.groupDoubleClicked(group());
  
  QGraphicsItem::mouseDoubleClickEvent(event);
}

void
GroupGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{

}
