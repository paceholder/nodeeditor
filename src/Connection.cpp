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
//: _id(QUuid())
  : _id(QUuid::createUuid())
  , _draggingEnd(EndType::NONE)
  , _connectionPainter(_connectionGeometry)
  , _connectionGraphicsObject(new ConnectionGraphicsObject(*this,
                                                           _connectionGeometry,
                                                           _connectionPainter))
{
//
}


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

      auto p = node->connectionPointScenePosition(address, _draggingEnd);
      connectToNode(address, p);

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

  auto p = _connectionGraphicsObject->mapFromScene(scenePoint);

  _connectionGeometry.setEndPoint(_draggingEnd, p);

  if (getAddress(oppositeEnd(_draggingEnd)).first.isNull())
  {
    _connectionGeometry.setEndPoint(oppositeEnd(_draggingEnd), p);
  }

  _connectionGraphicsObject->update();
}
