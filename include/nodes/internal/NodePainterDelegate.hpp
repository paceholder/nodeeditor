#pragma once

#include <QPainter>

#include "Export.hpp"

namespace QtNodes {

class NodeGraphicsObject;
class NodeIndex;
  
/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate
{

public:

  virtual
  ~NodePainterDelegate() = default;

  virtual void
  paint(QPainter* painter,
        NodeGeometry const& ngo,
        NodeIndex const& index) = 0;
};
}
