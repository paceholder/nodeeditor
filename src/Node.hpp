#ifndef NODE_HPP
#define NODE_HPP

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "NodeState.hpp"
#include "NodeGeometry.hpp"

class Node : public QObject
{
  Q_OBJECT

public:

  Node();
  ~Node();

public:

  QUuid id();

private:

  class NodeImpl;

  std::unique_ptr<NodeImpl> _impl;

};


#endif // NODE_HPP
