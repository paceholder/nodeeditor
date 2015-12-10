#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QUuid>

#include "EndType.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionGraphicsObject.hpp"

class Node;

//------------------------------------------------------------------------------

class Connection : QObject
{
  Q_OBJECT

public:
  Connection(std::pair<QUuid, int> address,
             EndType dragging);

  QUuid id() const { return _id; }

  void setDraggingEnd(EndType dragging);
  EndType draggingEnd() const { return _draggingEnd; }

  std::pair<QUuid, int> getAddress(EndType endType) const
  {
    switch (endType)
    {
      case EndType::SOURCE:
        return _sourceAddress;
        break;

      case EndType::SINK:
        return _sinkAddress;
        break;

      default:
        break;
    }
    return std::make_pair(QUuid(), 0);
  }

  void setAddress(EndType endType, std::pair<QUuid, int> address)
  {
    switch (endType)
    {
      case EndType::SOURCE:
        _sourceAddress = address;
        break;

      case EndType::SINK:
        _sinkAddress = address;
        break;

      default:
        break;
    }
  }

public:
  void tryConnectToNode(Node* node, QPointF const& scenePoint);

public slots:
  void onItemMoved();

private:
  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  // state
  EndType _draggingEnd;

private:

  // painting
  ConnectionGeometry _connectionGeometry;

  ConnectionPainter _connectionPainter;

  ConnectionGraphicsObject* _connectionGraphicsObject;
};

#endif // CONNECTION_H
