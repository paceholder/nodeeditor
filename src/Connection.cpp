#include "Connection.hpp"
#include <QtWidgets/QtWidgets>

#include "Node.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.hpp"

#include <iostream>
#include <math.h>

#define DEBUG_DRAWING 1

Connection::
Connection()
  : _id(QUuid())
  , _draggingEnd(EndType::NONE)
  , _connectionPainter(_connectionGeometry)
  , _connectionGraphicsObject(new ConnectionGraphicsObject(*this,
                                                           _connectionGeometry,
                                                           _connectionPainter))
{
//
}


//Connection::
//Connection(std::pair<QUuid, int> address, EndType draggingEnd)
  //: _id(QUuid::createUuid())
  //, _draggingEnd(draggingEnd)
  //, _connectionPainter(_connectionGeometry)
  //, _connectionGraphicsObject(new ConnectionGraphicsObject(*this,
                                                           //_connectionGeometry,
                                                           //_connectionPainter))
//{
  //{
    //Node* item = flowScene.getNode(address.first);
    //_connectionGraphicsObject->stackBefore(item);
  //}

  //QObject::connect(item, &Node::itemMoved, this, &Connection::onItemMoved);

  //_connectionGraphicsObject->grabMouse();

  //setAddress(dragging, address);

  //QPointF pointPos;
  //switch (_draggingEnd)
  //{
    //case  EndType::SOURCE:
    //{
      //_sinkAddress = address;
      //pointPos     = mapFromScene(item->connectionPointScenePosition(address, EndType::SINK));

      ////grabMouse();
      //break;
    //}

    //case EndType::SINK:
    //{
      //_sourceAddress = address;
      //pointPos       = mapFromScene(item->connectionPointScenePosition(address, EndType::SOURCE));

      ////grabMouse();
      //break;
    //}

    //default:
      //// should not get to here
      //break;
  //}

  //_source = pointPos;
  //_sink   = pointPos;
//}


void
Connection::
setDraggingEnd(EndType dragging)
{
  _draggingEnd = dragging;

  _connectionGraphicsObject->grabMouse();

  switch (_draggingEnd)
  {
    case EndType::SOURCE:
      _sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case EndType::SINK:
      _sinkAddress = std::make_pair(QUuid(), -1);
      break;

    default:
      break;
  }
}


//void
//Connection::
//advance(int )
//{
//// if (phase == 1) _animationPhase = (_animationPhase + 1) % 7;
//}

//void
//Connection::
//timerEvent(QTimerEvent*)
//{
//// this->advance(1);
//}

void
Connection::
tryConnectToNode(Node* node, QPointF const& scenePoint)
{
  bool ok = node->canConnect(_draggingEnd, scenePoint);

  if (ok)
  {
    auto address = node->connect(this, _draggingEnd, scenePoint);

    if (!address.first.isNull())
    {
      connectToNode(address, scenePoint);

      //------

      _draggingEnd = EndType::NONE;
    }
  }
}


void
Connection::
connectToNode(std::pair<QUuid, int> const &address,
              QPointF const& scenePoint)
{
  setAddress(_draggingEnd, address);

  _connectionGeometry.setEndPoint(_draggingEnd, scenePoint);
}
