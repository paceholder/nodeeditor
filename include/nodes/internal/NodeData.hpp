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

inline
bool
operator<(QtNodes::NodeDataType const & d1,
          QtNodes::NodeDataType const & d2)
{
  return d1.id < d2.id;
}

inline
bool
operator==(QtNodes::NodeDataType const & d1,
           QtNodes::NodeDataType const & d2)
{
  return d1.id == d2.id;
}

inline
bool
operator!=(QtNodes::NodeDataType const & d1,
           QtNodes::NodeDataType const & d2)
{
  return !operator==(d1,d2);
}

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
