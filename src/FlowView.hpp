#pragma once

#include <QtWidgets/QGraphicsView>

#include "Export.hpp"

namespace QtNodes
{

class FlowScene;

class NODE_EDITOR_PUBLIC FlowView
  : public QGraphicsView
{
public:

  FlowView(FlowScene *scene);

public slots:

  void scaleUp();

  void scaleDown();

protected:

  void contextMenuEvent(QContextMenuEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;

  void showEvent(QShowEvent *event) override;

private:

  FlowScene* _scene;
};
}
