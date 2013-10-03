#ifndef FLOW_ITEM_H
#define FLOW_ITEM_H

#include <QGraphicsObject>

class FlowItem: public QGraphicsObject
{
  Q_OBJECT

public:
  FlowItem();

  QRectF
  boundingRect() const override;

  /* *
   * EVENTS
   *
   * mouseOverIn
   * mouseOverOut
   *
   * mouseReleaseIn
   * mouseReleaseOut
   *
   * mousePressIn
   * mousePressOut
   *
   * */

private:
  int _width;
  int _height;
};

#endif // FLOW_ITEM_H
