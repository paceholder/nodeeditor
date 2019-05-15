#pragma once

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "Export.hpp"

#include <memory>

namespace QtNodes
{

using NodeDataTypeId = QString;

class NodeDataType
{
public:
  NodeDataType() = default;

  virtual ~NodeDataType() = default;

  NodeDataType(const QString& id, const QString& name)
    : _id(id)
    , _name(name)
  {

  }

  virtual bool operator ==(const NodeDataType& other) const
  {
    return id() == other.id();
  }

  virtual bool operator !=(const NodeDataType& other) const
  {
    return !(*this == other);
  }

  virtual bool operator <(const NodeDataType& other) const
  {
    return id() < other.id();
  }

  NodeDataTypeId id() const
  {
    return _id;
  }

  QString name() const
  {
    return _name;
  }

private:
  NodeDataTypeId _id;
  QString _name;
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
    return (this->type() == nodeData.type());
  }

  /// Type for inner use
  virtual std::shared_ptr<NodeDataType> type() const = 0;
};
}
