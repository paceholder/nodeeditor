#ifndef NODE_DATA_HPP
#define NODE_DATA_HPP

#include <QtCore/QString>

/// Class represents data transferred between nodes.
/// @param type is used for comparing the types
/// The actual data is stored in subtypes
class NodeData
{
public:
  virtual bool sameType(NodeData const &nodeDataType) const = 0;

  virtual QString type() const = 0;
};

#endif //  NODE_DATA_HPP
