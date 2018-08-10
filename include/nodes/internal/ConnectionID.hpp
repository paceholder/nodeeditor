#pragma once

#include "NodeIndex.hpp"
#include "PortType.hpp"

#include <utility>

namespace QtNodes {
struct ConnectionID {
  QUuid lNodeID;
  QUuid rNodeID;

  PortIndex lPortID;
  PortIndex rPortID;
};

inline bool operator==(ConnectionID const& lhs, ConnectionID const& rhs) {
  return lhs.lNodeID == rhs.lNodeID &&
         lhs.rNodeID == rhs.rNodeID &&
         lhs.lPortID == rhs.lPortID &&
         lhs.rPortID == rhs.rPortID;
}

} // namespace QtNodes

// hash for ConnectionID
namespace std {

template<>
struct hash<::QtNodes::ConnectionID> {
  size_t operator()(::QtNodes::ConnectionID const& toHash) const {
    return qHash(toHash.rNodeID) ^ qHash(toHash.lNodeID) ^ std::hash<QtNodes::PortIndex>()(toHash.lPortID) ^ std::hash<QtNodes::PortIndex>()(toHash.rPortID);
  }
};

} // namespace std

