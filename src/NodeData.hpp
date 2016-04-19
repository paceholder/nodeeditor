#pragma once

#include <QtCore/QString>

/// Class represents data transferred between nodes.
/// @param type is used for comparing the types
/// The actual data is stored in subtypes
class NodeData
{
public:
  virtual bool sameType(NodeData const &nodeData) const
  {
    return (this->type() == nodeData.type());
  }

  /// Type for inner use
  virtual QString type() const = 0;

  /// Name for GUI
  virtual QString name() const = 0;
};
