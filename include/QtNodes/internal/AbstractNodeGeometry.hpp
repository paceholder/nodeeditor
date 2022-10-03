#pragma once

#include "Export.hpp"
#include "Definitions.hpp"

#include <QRectF>
#include <QSize>
#include <QTransform>

namespace QtNodes
{

class AbstractGraphModel;

class NODE_EDITOR_PUBLIC AbstractNodeGeometry
{
public:
  AbstractNodeGeometry(AbstractGraphModel &);
  virtual ~AbstractNodeGeometry() {}

  virtual QRectF boundingRect(NodeId const nodeId) const = 0;

  virtual QSize size(NodeId const nodeId) const = 0;

  virtual void recomputeSize(NodeId const nodeId) const = 0;

  virtual
  QPointF
  portPosition(NodeId const    nodeId,
               PortType const  portType,
               PortIndex const index) const = 0;

  virtual
  QPointF
  portScenePosition(NodeId const       nodeId,
                    PortType const     portType,
                    PortIndex const    index,
                    QTransform const & t) const;

  virtual
  QPointF
  portTextPosition(NodeId const   nodeId,
                   PortType const portType,
                   PortIndex const portIndex) const = 0;

  virtual
  QPointF
  captionPosition(NodeId const nodeId) const = 0;

  virtual
  QRectF
  captionRect(NodeId const nodeId) const = 0;

  virtual
  QPointF
  widgetPosition(NodeId const nodeId) const = 0;

  virtual
  PortIndex
  checkPortHit(NodeId const   nodeId,
               PortType const portType,
               QPointF const  nodePoint) const;

  virtual
  QRect
  resizeHandleRect(NodeId const nodeId) const = 0;

protected:
  AbstractGraphModel & _graphModel;
};

}
