#include "ConnectionBlurEffect.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionPainter.hpp"

using QtNodes::ConnectionBlurEffect;
using QtNodes::ConnectionGraphicsObject;

ConnectionBlurEffect::
ConnectionBlurEffect(ConnectionGraphicsObject* object)
  : _object(object)
{
  //
}


void
ConnectionBlurEffect::
draw(QPainter* painter)
{
  QGraphicsBlurEffect::draw(painter);

  //ConnectionPainter::paint(painter,
  //_object->connectionGeometry(),
  //_object->connectionState());

  //_item->paint(painter, nullptr, nullptr);
}
