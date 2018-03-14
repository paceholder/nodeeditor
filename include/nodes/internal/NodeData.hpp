#pragma once

#include <QtCore/QString>

#include "Export.hpp"

namespace QtNodes
{

struct NodeDataType
{
  QString id;
  QString name;
};

/// Class represents data transferred between nodes.
/// @param type is used for comparing the types
/// The actual data is stored in subtypes
class NODE_EDITOR_PUBLIC NodeData
{
public:

  virtual ~NodeData() = default;

  virtual bool sameType(NodeData const &nodeData) const
  {
    return (this->type().id == nodeData.type().id);
  }

  /// Type for inner use
  virtual NodeDataType type() const = 0;
};
}
