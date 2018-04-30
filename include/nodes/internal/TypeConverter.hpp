#pragma once

#include "NodeData.hpp"
#include "memory.hpp"

#include <functional>

namespace QtNodes
{

using SharedNodeData = std::shared_ptr<NodeData>;

// a function taking in NodeData and returning NodeData
using TypeConverter =
  std::function<SharedNodeData(SharedNodeData)>;

// data-type-in, data-type-out
using TypeConverterId =
  std::pair<NodeDataType, NodeDataType>;

}
