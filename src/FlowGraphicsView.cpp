#include "FlowGraphicsView.hpp"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QWheelEvent>
#include <QtCore/QRectF>

#include <QtOpenGL>
#include <QtWidgets>

#include <QDebug>
#include <iostream>

FlowGraphicsView::
FlowGraphicsView(QGraphicsScene *scene)
  : QGraphicsView(scene)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);
  setBackgroundBrush(QColor(Qt::gray).darker(300));

  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  //setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);

  //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}


void
FlowGraphicsView::
wheelEvent(QWheelEvent *event)
{
  QPoint delta   = event->angleDelta();
  double const d = delta.y() / std::abs(delta.y());

  double const step = 1.2;

  double const factor = std::pow(step, d);

  QTransform t = transform();

  if (t.m11() > 2.0 && factor > 1.0)
    return;

  scale(factor, factor);
}


void
FlowGraphicsView::
drawBackground(QPainter* painter, const QRectF& r)
{
  QGraphicsView::drawBackground(painter, r);

  auto drawGrid =
    [&](double gridStep)
    {
      QRect   windowRect = rect();
      QPointF tl = mapToScene(windowRect.topLeft());
      QPointF br = mapToScene(windowRect.bottomRight());

      double left   = std::floor(tl.x() / gridStep - 0.5);
      double right  = std::floor(br.x() / gridStep + 1.0);
      double bottom = std::floor(tl.y() / gridStep - 0.5);
      double top    = std::floor (br.y() / gridStep + 1.0);

      // vertical lines
      for (int xi = int(left); xi <= int(right); ++xi)
      {
        QLineF line(xi * gridStep, bottom * gridStep,
                    xi * gridStep, top * gridStep );

        painter->drawLine(line);
      }

      // horizontal lines
      for (int yi = int(bottom); yi <= int(top); ++yi)
      {
        QLineF line(left * gridStep, yi * gridStep,
                    right * gridStep, yi * gridStep );
        painter->drawLine(line);
      }
    };

  QBrush bBrush    = backgroundBrush();
  QColor gridColor = bBrush.color().lighter(120);

  QPen pfine(gridColor, 1.0);

  painter->setPen(pfine);
  drawGrid(15);

  gridColor = bBrush.color().darker(200);
  QPen p(gridColor, 1.0);

  painter->setPen(p);
  drawGrid(150);
}
