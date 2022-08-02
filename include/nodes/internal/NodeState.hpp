#pragma once

#include <vector>
#include <unordered_map>

#include <QtCore/QPointF>
#include <QtCore/QUuid>

#include "Export.hpp"

#include "Definitions.hpp"
#include "NodeData.hpp"

namespace QtNodes
{

class NodeGraphicsObject;

/// Contains vectors of connected input and output connections.
/// Stores bool for reacting on hovering connections
class NODE_EDITOR_PUBLIC NodeState
{
public:

  enum ReactToConnectionState
  {
    REACTING,
    NOT_REACTING
  };

  //using ConnectionPtrSet =
    //std::unordered_map<QUuid, Connection*>;

public:

  NodeState(NodeGraphicsObject & ngo);

public:


  /// Returns vector of connections ID.
  /// Some of them can be empty (null)
  //std::vector<ConnectionGraphicsObject const*> const&
  //getEntries(PortType) const;

  //std::vector<ConnectionGraphicsObject*> &
  //getEntries(PortType);

  //ConnectionPtrSet
  //connections(PortType portType, PortIndex portIndex) const;

  //void
  //setConnection(PortType portType,
                //PortIndex portIndex,
                //ConnectionId & connection);

  //void
  //eraseConnection(PortType portType,
                  //PortIndex portIndex,
                  //QUuid id);

public:

  bool hovered() const { return _hovered; }

  void setHovered(bool hovered = true) { _hovered = hovered; }


  void
  setResizing(bool resizing);

  bool
  resizing() const;

  //void lock(bool locked = true);

  //void unlock() { lock(false); }

public:

  ReactToConnectionState
  reaction() const;

  PortType
  reactingPortType() const;

  NodeDataType
  reactingDataType() const;

  QPointF draggingPos() const { return _draggingPos; }

  void
  setReaction(ReactToConnectionState reaction,
              PortType reactingPortType = PortType::None,
              NodeDataType reactingDataType = NodeDataType());

  bool
  isReacting() const;

  void reactToPossibleConnection(PortType,
                                 NodeDataType const &,
                                 QPointF const & scenePoint);

  void resetReactionToConnection();

private:

  NodeGraphicsObject & _ngo;

  bool _hovered;

  //std::vector<ConnectionPtrSet> _inConnections;
  //std::vector<ConnectionPtrSet> _outConnections;

  ReactToConnectionState _reaction;
  PortType _reactingPortType;
  NodeDataType _reactingDataType;

  QPointF _draggingPos;

  bool _locked; // TODO: WTF?

  bool _resizing;
};
}
