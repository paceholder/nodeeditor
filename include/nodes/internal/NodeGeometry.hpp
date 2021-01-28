#pragma once

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QTransform>
#include <QtGui/QFontMetrics>
#include <QIcon>

#include "PortType.hpp"
#include "Export.hpp"
#include "memory.hpp"


namespace QtNodes
{

class NodeDataModel;
class NodeState;
class Node;
enum class NodeProcessingStatus;
/**
 * @brief The NodeGeometry class holds the aspects of a node's graphical object
 * geometry in the FlowScene, such as the position of each port within a node.
 * Each connection is associated with a unique geometry object.
 */
class NODE_EDITOR_PUBLIC NodeGeometry
{
public:

  NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel);

public:
  unsigned int
  height() const
  {
    return _height;
  }

  void
  setHeight(unsigned int h)
  {
    _height = h;
  }

  unsigned int
  width() const
  {
    return _width;
  }

  void
  setWidth(unsigned int w)
  {
    _width = w;
  }

  unsigned int
  entryHeight() const
  {
    return _entryHeight;
  }
  void
  setEntryHeight(unsigned int h)
  {
    _entryHeight = h;
  }

  unsigned int
  entryWidth() const
  {
    return _entryWidth;
  }

  void
  setEntryWidth(unsigned int w)
  {
    _entryWidth = w;
  }

  unsigned int
  spacing() const
  {
    return _spacing;
  }

  void
  setSpacing(unsigned int s)
  {
    _spacing = s;
  }

  bool
  hovered() const
  {
    return _hovered;
  }

  void
  setHovered(unsigned int h)
  {
    _hovered = h;
  }

  unsigned int
  nSources() const;

  unsigned int
  nSinks() const;

  QPointF const&
  draggingPos() const
  {
    return _draggingPos;
  }

  void
  setDraggingPosition(QPointF const& pos)
  {
    _draggingPos = pos;
  }

public:

  QRectF
  entryBoundingRect() const;

  QRectF
  boundingRect() const;

  /// Updates size unconditionally
  void
  recalculateSize() const;

  /// Updates size if the QFontMetrics is changed
  void
  recalculateSize(QFont const &font) const;

  // TODO removed default QTransform()
  QPointF
  portScenePosition(PortIndex index,
                    PortType portType,
                    QTransform const & t = QTransform()) const;

  PortIndex
  checkHitScenePoint(PortType portType,
                     QPointF point,
                     QTransform const & t = QTransform()) const;

  QRect
  resizeRect() const;

  /// Returns the position of a widget on the Node surface
  QPointF
  widgetPosition() const;

  /// Returns the maximum height a widget can be without causing the node to grow.
  int
  equivalentWidgetHeight() const;

  unsigned int
  validationHeight() const;

  unsigned int
  validationWidth() const;

  static
  QPointF
  calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node* targetNode,
                                        PortIndex sourcePortIndex, PortType sourcePort, Node* sourceNode,
                                        Node& newNode);

  /**
   * @brief Updates the space reserved for the status icon based on the
   * model's current status.
   */
  void
  updateStatusIconSize() const;

  /**
   * @brief Returns the size (width and height) of the icon that indicates
   * the node's current processing status.
   */
  QSize
  statusIconSize() const;

  /**
   * @brief Returns the dimensions of the icon that indicates the node's
   * current processing status.
   */
  QRect
  statusIconRect() const;

  /**
   * @brief Returns the icon associated with the model's current processing status.
   */
  const QIcon&
  processingStatusIcon() const;

private:

  unsigned int
  captionHeight() const;

  unsigned int
  captionWidth() const;

  unsigned int
  nicknameHeight() const;

  unsigned int
  nicknameWidth() const;

  unsigned int
  portWidth(PortType portType) const;

private:

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable unsigned int _width;
  mutable unsigned int _height;
  unsigned int _entryWidth;
  mutable unsigned int _inputPortWidth;
  mutable unsigned int _outputPortWidth;
  mutable unsigned int _entryHeight;
  mutable QSize _statusIconSize;
  unsigned int _spacing;

  bool _hovered;

  /**
   * @brief Flag indicating whether the processing status icon should be part
   * of the node's geometry.
   */
  mutable bool _statusIconActive;

  QPointF _draggingPos;

  std::unique_ptr<NodeDataModel> const &_dataModel;

  mutable QFontMetrics _fontMetrics;
  mutable QFontMetrics _boldFontMetrics;

  /**
   * @brief Processing status icons
   */
  const QIcon _statusUpdated{"://status_icons/updated.svg"};
  const QIcon _statusProcessing{"://status_icons/processing.svg"};
  const QIcon _statusPending{"://status_icons/pending.svg"};
  const QIcon _statusInvalid{"://status_icons/failed.svg"};
  const QIcon _statusEmpty{"://status_icons/empty.svg"};
  const QIcon _statusPartial{"://status_icons/partial.svg"};

};
}
