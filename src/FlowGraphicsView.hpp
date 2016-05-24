#pragma once

#include <QtWidgets/QGraphicsView>

class FlowScene;

class FlowGraphicsView : public QGraphicsView
{
public:

  FlowGraphicsView(FlowScene *scene);

protected:

  void contextMenuEvent(QContextMenuEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;

private:

  FlowScene* _scene;
};
