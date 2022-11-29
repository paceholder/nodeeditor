#pragma once

#include <QtWidgets/QGraphicsView>

#include "Export.hpp"

namespace QtNodes
{

class BasicGraphicsScene;

class NODE_EDITOR_PUBLIC GraphicsView
  : public QGraphicsView
{
  Q_OBJECT
public:
  struct ScaleRange
  {
    double minimum = 0;
    double maximum = 0;
  };

  GraphicsView(QWidget *parent = Q_NULLPTR);
  GraphicsView(BasicGraphicsScene *scene, QWidget *parent = Q_NULLPTR);

  GraphicsView(const GraphicsView &) = delete;
  GraphicsView
  operator=(const GraphicsView &) = delete;

  QAction*
  clearSelectionAction() const;

  QAction*
  deleteSelectionAction() const;

  void
  setScene(BasicGraphicsScene *scene);

  void
  centerScene();

  /// @brief max=0/min=0 indicates infinite zoom in/out
  void
  setScaleRange(double minimum = 0, double maximum = 0);

  void
  setScaleRange(ScaleRange range);

public Q_SLOTS:
  void
  scaleUp();

  void
  scaleDown();

  void
  setupScale(double scale);

  void
  onDeleteSelectedObjects();

  void
  onDuplicateSelectedObjects();

protected:
  void
  contextMenuEvent(QContextMenuEvent *event) override;

  void
  wheelEvent(QWheelEvent *event) override;

  void
  keyPressEvent(QKeyEvent *event) override;

  void
  keyReleaseEvent(QKeyEvent *event) override;

  void
  mousePressEvent(QMouseEvent *event) override;

  void
  mouseMoveEvent(QMouseEvent *event) override;

  void
  drawBackground(QPainter* painter, const QRectF & r) override;

  void
  showEvent(QShowEvent *event) override;

protected:
  BasicGraphicsScene *
  nodeScene();

private:
  QAction* _clearSelectionAction;
  QAction* _deleteSelectionAction;
  QAction* _duplicateSelectionAction;

  QPointF _clickPos;
  ScaleRange _scaleRange;
};
}
