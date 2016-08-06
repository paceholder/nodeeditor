#pragma once

#include <QtCore/QString>


struct NodeDataType
{
  QString id;
  QString name;
};

/// Class represents data transferred between nodes.
/// @param type is used for comparing the types
/// The actual data is stored in subtypes
class NodeData
{
public:
  virtual bool sameType(NodeData const &nodeData) const
  {
    return (this->type().id == nodeData.type().id);
  }

  /// Type for inner use
  virtual NodeDataType type() const = 0;
};
