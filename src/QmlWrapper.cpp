#include "QmlWrapper.hpp"

#include <QtNodes/NodeDelegateModelRegistry>

#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

using QtNodes::NodeDelegateModelRegistry;

namespace QtNodes {

QuickPaintedView::QuickPaintedView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //! NOTE It is necessary that when UI scaling is displayed without a blur
    setAntialiasing(false);
    setSmooth(false);
}

QSGNode *QuickPaintedView::updatePaintNode(QSGNode *old, UpdatePaintNodeData *data)
{
    //! NOTE It is necessary that when UI scaling is displayed without a blur
    setTextureSize(QSize(width(), height()));
    QSGNode *n = QQuickPaintedItem::updatePaintNode(old, data);
    update(); // Endless loop
    return n;
}

QmlWrapper::QmlWrapper(QQuickItem *parent)
    : QuickPaintedView(parent)
{
    setFlag(QQuickItem::ItemAcceptsDrops, true);
    setFlag(QQuickItem::ItemHasContents, true);

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

void QmlWrapper::paint(QPainter *painter)
{
    if (qWidget()) {
        qWidget()->render(painter,
                          QPoint(),
                          QRegion(),
                          QWidget::DrawWindowBackground | QWidget::DrawChildren);
    }
}

bool QmlWrapper::event(QEvent *event)
{
    if (!m_widget) {
        return QQuickItem::event(event);
    }

    bool ok = true;

    switch (event->type()) {
    case QEvent::HoverEnter:
    case QEvent::HoverMove:
    case QEvent::HoverLeave:
        ok = handleHoverEvent(dynamic_cast<QHoverEvent *>(event));
        break;
    case QEvent::MouseButtonPress: {
        setFocus(true);
        ok = m_widget->handleEvent(event);
        break;
    }
    default:
        ok = m_widget->handleEvent(event);
        break;
    }

    if (ok) {
        update();
    }

    return ok;
}

bool QmlWrapper::handleHoverEvent(QHoverEvent *event)
{
    auto convertEventType = [](QEvent::Type type) {
        static const QMap<QEvent::Type, QEvent::Type> types{{QEvent::HoverLeave, QEvent::Leave},
                                                            {QEvent::HoverEnter, QEvent::Enter},
                                                            {QEvent::HoverMove, QEvent::MouseMove}};

        return types[type];
    };

    QEvent::Type convertedType = convertEventType(event->type());

    if (convertedType == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = new QMouseEvent(convertedType,
                                                  event->position(),
                                                  event->scenePosition(),
                                                  event->globalPosition(),
                                                  event->button(),
                                                  event->buttons(),
                                                  event->modifiers(),
                                                  Qt::MouseEventSource::MouseEventNotSynthesized,
                                                  event->pointingDevice());
        mouseEvent->setAccepted(event->isAccepted());
        mouseEvent->setTimestamp(event->timestamp());
        bool ok = m_widget->handleEvent(mouseEvent);
        setCursor(m_widget->cursor());
        return ok;
    }

    QEvent newEvent(convertedType);
    newEvent.setAccepted(event->isAccepted());

    return QCoreApplication::sendEvent(m_widget, &newEvent);
}

void QmlWrapper::componentComplete()
{
    QQuickItem::componentComplete();

    setWidget();

    connect(this, &QQuickItem::widthChanged, [this]() { updateSizeConstraints(); });

    connect(this, &QQuickItem::heightChanged, [this]() { updateSizeConstraints(); });
}

QWidget *QmlWrapper::qWidget() const
{
    return m_widget;
}

void QmlWrapper::updateSizeConstraints()
{
    if (qWidget()) {
        qWidget()->setFixedSize(width(), height());
    }
}

void QmlWrapper::setWidget()
{
    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    m_model = new DataFlowGraphModel(registry);

    m_widget_scene = new DataFlowGraphicsScene(*m_model, nullptr);

    m_widget = new GraphicsView(m_widget_scene);

    updateSizeConstraints();

    QObject::connect(m_widget_scene,
                     &DataFlowGraphicsScene::sceneLoaded,
                     m_widget,
                     &GraphicsView::centerScene);
}

} // namespace QtNodes
