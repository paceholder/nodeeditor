#include "Connection.hpp"

#include <iostream>
#include <math.h>

#include <QtWidgets/QtWidgets>

#include "Node.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.hpp"

#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionGraphicsObject.hpp"

class
Connection::ConnectionImpl
{
public:
  ConnectionImpl(Connection* connection)
    : _id(QUuid::createUuid())
    , _draggingEnd(EndType::NONE)
    , _connectionPainter(_connectionGeometry)
    , _connectionGraphicsObject(new ConnectionGraphicsObject(*connection,
                                                             _connectionGeometry,
                                                             _connectionPainter))
  {}

public:
  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  // state
  EndType _draggingEnd;

  // painting
  ConnectionGeometry _connectionGeometry;

  ConnectionPainter _connectionPainter;

  ConnectionGraphicsObject* _connectionGraphicsObject;
};

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

Connection::
Connection()
  : _impl(new ConnectionImpl(this))
{
//
}


Connection::
~Connection()
{}

QUuid
Connection::
id() const
{
  return _impl->_id;
}


void
Connection::
setDraggingEnd(EndType dragging)
{
  _impl->_draggingEnd = dragging;

  _impl->_connectionGraphicsObject->grabMouse();

  switch (_impl->_draggingEnd)
  {
    case EndType::SOURCE:
      _impl->_sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case EndType::SINK:
      _impl->_sinkAddress = std::make_pair(QUuid(), -1);
      break;

    default:
      break;
  }
}


EndType
Connection::
draggingEnd() const
{
  return _impl->_draggingEnd;
}


std::pair<QUuid, int>
Connection::
getAddress(EndType endType) const
{
  switch (endType)
  {
    case EndType::SOURCE:
      return _impl->_sourceAddress;
      break;

    case EndType::SINK:
      return _impl->_sinkAddress;
      break;

    default:
      break;
  }
  return std::make_pair(QUuid(), 0);
}


void
Connection::
setAddress(EndType endType, std::pair<QUuid, int> address)
{
  switch (endType)
  {
    case EndType::SOURCE:
      _impl->_sourceAddress = address;
      break;

    case EndType::SINK:
      _impl->_sinkAddress = address;
      break;

    default:
      break;
  }
}


void
Connection::
tryConnectToNode(Node* node, QPointF const& scenePoint)
{
  bool ok = node->canConnect(_impl->_draggingEnd, scenePoint);

  if (ok)
  {
    auto address = node->connect(this, _impl->_draggingEnd, scenePoint);

    if (!address.first.isNull())
    {

      auto p = node->connectionPointScenePosition(address, _impl->_draggingEnd);
      connectToNode(address, p);

      //------

      _impl->_draggingEnd = EndType::NONE;
    }
  }
}


void
Connection::
connectToNode(std::pair<QUuid, int> const &address,
              QPointF const& scenePoint)
{
  setAddress(_impl->_draggingEnd, address);

  auto p = _impl->_connectionGraphicsObject->mapFromScene(scenePoint);

  _impl->_connectionGeometry.setEndPoint(_impl->_draggingEnd, p);

  if (getAddress(oppositeEnd(_impl->_draggingEnd)).first.isNull())
  {
    _impl->_connectionGeometry.setEndPoint(oppositeEnd(_impl->_draggingEnd), p);
  }

  _impl->_connectionGraphicsObject->update();
}


ConnectionGraphicsObject*
Connection::
getConnectionGraphicsObject() const
{
  return _impl->_connectionGraphicsObject;
}


ConnectionGeometry&
Connection::
connectionGeometry()
{
  return _impl->_connectionGeometry;
}
