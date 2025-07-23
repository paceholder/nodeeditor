#pragma once

#include <QtWidgets/QGraphicsView>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QTest>

namespace UITestHelper
{
    inline void simulateMousePress(QGraphicsView* view, QPointF scenePos, Qt::MouseButton button = Qt::LeftButton)
    {
        QPointF viewPos = view->mapFromScene(scenePos);
        QMouseEvent pressEvent(QEvent::MouseButtonPress, viewPos.toPoint(), 
                              view->mapToGlobal(viewPos.toPoint()), button, button, Qt::NoModifier);
        QApplication::sendEvent(view->viewport(), &pressEvent);
    }

    inline void simulateMouseMove(QGraphicsView* view, QPointF scenePos)
    {
        QPointF viewPos = view->mapFromScene(scenePos);
        QMouseEvent moveEvent(QEvent::MouseMove, viewPos.toPoint(), 
                             view->mapToGlobal(viewPos.toPoint()), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(view->viewport(), &moveEvent);
    }

    inline void simulateMouseRelease(QGraphicsView* view, QPointF scenePos, Qt::MouseButton button = Qt::LeftButton)
    {
        QPointF viewPos = view->mapFromScene(scenePos);
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, viewPos.toPoint(), 
                                view->mapToGlobal(viewPos.toPoint()), button, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(view->viewport(), &releaseEvent);
    }

    inline void simulateMouseDrag(QGraphicsView* view, QPointF fromScene, QPointF toScene)
    {
        simulateMousePress(view, fromScene);
        QTest::qWait(10); // Small delay for realism
        simulateMouseMove(view, toScene);
        QTest::qWait(10);
        simulateMouseRelease(view, toScene);
        QTest::qWait(10);
    }

    inline void waitForUI(int ms = 10)
    {
        QTest::qWait(ms);
        QApplication::processEvents();
    }
}
