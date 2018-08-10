#pragma once

#include <cstddef>
#include <limits>

#include <QUuid>
#include <QtGlobal>

namespace QtNodes {

class FlowSceneModel;

class NodeIndex {
  friend FlowSceneModel;
public:

  /// Construct an invalid NodeIndex
  NodeIndex() = default;

private:
  /// Regular constructor
  NodeIndex(const QUuid& uuid, void* internalPtr, const FlowSceneModel* model) 
    : _id {uuid}, _internalPointer{internalPtr}, _model{model} {
      Q_ASSERT(!_id.isNull());
      Q_ASSERT(_model != nullptr);
    }

public:

  /// Get the internal pointer
  void* internalPointer() const { return _internalPointer; }

  /// Get the owning model
  const FlowSceneModel* model() const { return _model; }

  /// Get the id for the node
  QUuid id() const { return _id; }
  
  /// Test if it's valid
  bool isValid() const {
    return !id().isNull() && model() != nullptr;
  }


private:

  QUuid _id;
  void* _internalPointer = nullptr;
  const FlowSceneModel* _model = nullptr;
};

inline bool operator==(NodeIndex const& lhs, NodeIndex const& rhs) {
  return lhs.model() == rhs.model() &&
         lhs.id()    == rhs.id();
}

inline bool operator!=(NodeIndex const& lhs, NodeIndex const& rhs) {
  return !(lhs == rhs);
}

} // namespace QtNodes

