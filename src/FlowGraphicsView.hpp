#pragma once

#include <QtWidgets/QGraphicsView>

#include "Export.hpp"

class FlowScene;

class NODE_EDITOR_PUBLIC FlowGraphicsView
  : public QGraphicsView
{
public:

  FlowGraphicsView(FlowScene *scene);

public slots:

  void scaleUp();

  void scaleDown();

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
  drawBackground(QPainter* painter, const QRectF& r) override;

private:

  FlowScene* _scene;
};
