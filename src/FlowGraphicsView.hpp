#pragma once

#include <QtWidgets/QGraphicsView>

class FlowGraphicsView : public QGraphicsView
{
public:

  FlowGraphicsView(QGraphicsScene *scene);

protected:

  void contextMenuEvent(QContextMenuEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;
};
