#include "FlowView.hpp"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtWidgets/QMenu>

#include <QtCore/QRectF>

#include <QtOpenGL>
#include <QtWidgets>

#include <QDebug>
#include <iostream>

#include "FlowScene.hpp"

#include "DataModelRegistry.hpp"

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

FlowView::
FlowView(FlowScene *scene)
  : QGraphicsView(scene)
  , _scene(scene)
{
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);
  setBackgroundBrush(QColor(Qt::gray).darker(300));

  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  //setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);

  //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}


void
FlowView::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu modelMenu;

  for (auto const &modelRegistry : DataModelRegistry::registeredModels())
  {
    QString const &modelName = modelRegistry.first;
    modelMenu.addAction(modelName);
  }

  if (QAction * action = modelMenu.exec(event->globalPos()))
  {
    //qDebug() << action->text();

    QString modelName = action->text();

    auto const &models =
      DataModelRegistry::registeredModels();

    auto it = models.find(modelName);

    if (it != models.end())
    {
      auto node = _scene->createNode(it->second->create() );

      QPoint pos = event->pos();

      QPointF posView = this->mapToScene(pos);

      node->nodeGraphicsObject()->setPos(posView);
    }
    else
    {
      qDebug() << "Model not found";
    }
  }
}


void
FlowView::
wheelEvent(QWheelEvent *event)
{
  QPoint delta = event->angleDelta();

  if (delta.y() == 0)
  {
    event->ignore();
    return;
  }

  double const d = delta.y() / std::abs(delta.y());

  if (d > 0.0)
    scaleUp();
  else
    scaleDown();
}


void
FlowView::
scaleUp()
{
  double const step   = 1.2;
  double const factor = std::pow(step, 1.0);

  QTransform t = transform();

  if (t.m11() > 2.0)
    return;

  scale(factor, factor);
}


void
FlowView::
scaleDown()
{
  double const step   = 1.2;
  double const factor = std::pow(step, -1.0);

  scale(factor, factor);
}


void
FlowView::
keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
    case Qt::Key_Escape:
      _scene->clearSelection();
      break;

    case Qt::Key_Delete:
      for (QGraphicsItem * item : _scene->selectedItems())
      {
        _scene->removeNode(item);
      }

      break;

    case Qt::Key_Shift:
      setDragMode(QGraphicsView::RubberBandDrag);
      break;

    default:
      break;
  }

  QGraphicsView::keyPressEvent(event);
}


void
FlowView::
keyReleaseEvent(QKeyEvent *event)
{
  switch (event->key())
  {
    case Qt::Key_Shift:
      setDragMode(QGraphicsView::ScrollHandDrag);
      break;

    default:
      break;
  }
  QGraphicsView::keyReleaseEvent(event);
}


void
FlowView::
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


void
FlowView::
showEvent(QShowEvent *event)
{
  _scene->setSceneRect(this->rect());
  QGraphicsView::showEvent(event);
}


void
FlowView::
mousePressEvent(QMouseEvent* event)
{
  QGraphicsView::mousePressEvent(event);
}


void
FlowView::
mouseMoveEvent(QMouseEvent* event)
{
  QGraphicsView::mouseMoveEvent(event);
}
