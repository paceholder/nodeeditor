#ifndef NODE_DATA_TYPE_HPP
#define NODE_DATA_TYPE_HPP

#include <QtCore/QString>

class NodeDataType
{
public:
  virtual bool sameType(NodeDataType const &nodeDataType) const = 0;

  virtual QString toString() const = 0;
};

#endif //  NODE_DATA_TYPE_HPP
