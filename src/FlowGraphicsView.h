#ifndef FLOW_GRAPHICS_VIEW_H
#define FLOW_GRAPHICS_VIEW_H

#include <QtWidgets/QGraphicsView>

class FlowGraphicsView : public QGraphicsView
{
public:

  FlowGraphicsView(QGraphicsScene *scene);

protected:

  void wheelEvent(QWheelEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;
};

#endif //  FLOW_GRAPHICS_VIEW_H
