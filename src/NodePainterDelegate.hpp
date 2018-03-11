#pragma once

#include <QPainter>

#include "NodeGeometry.hpp"
#include "NodeDataModel.hpp"
#include "Export.hpp"

namespace QtNodes {

/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate
{

public:

  virtual
  ~NodePainterDelegate() {}

  virtual void
  paint(QPainter* painter,
        NodeGeometry const& geom,
        NodeDataModel const * model) = 0;
};
}
