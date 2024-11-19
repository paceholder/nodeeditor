#include "QmlWrapper.hpp"
#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "StringDataModel.hpp"
#include "SubtractionModel.hpp"
#include <initializer_list>
#include <memory>
#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QIODevice>
#include <QIODeviceBase>
#include <QJsonDocument>
#include <QMap>
#include <QPoint>
#include <QRegion>
#include <QSize>
#include <Qt>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

using QtNodes::NodeDelegateModelRegistry;

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

WidgetView::WidgetView(QQuickItem *parent)
    : QuickPaintedView(parent)
{
    setFlag(QQuickItem::ItemAcceptsDrops, true);
    setFlag(QQuickItem::ItemHasContents, true);

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

void WidgetView::paint(QPainter *painter)
{
    if (qWidget()) {
        qWidget()->render(painter,
                          QPoint(),
                          QRegion(),
                          QWidget::DrawWindowBackground | QWidget::DrawChildren);
    }
}

bool WidgetView::event(QEvent *event)
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

bool WidgetView::handleHoverEvent(QHoverEvent *event)
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

void WidgetView::componentComplete()
{
    QQuickItem::componentComplete();

    setWidget();

    connect(this, &QQuickItem::widthChanged, [this]() { updateSizeConstraints(); });

    connect(this, &QQuickItem::heightChanged, [this]() { updateSizeConstraints(); });
}

QWidget *WidgetView::qWidget() const
{
    //return m_widget ? m_widget->qWidget() : nullptr;
    return m_widget;
}

void WidgetView::updateSizeConstraints()
{
    if (qWidget()) {
        qWidget()->setFixedSize(width(), height());
    }
}

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<NumberSourceDataModel>("Sources");

    ret->registerModel<NumberDisplayDataModel>("Displays");

    ret->registerModel<AdditionModel>("Operators");

    ret->registerModel<SubtractionModel>("Operators");

    ret->registerModel<MultiplicationModel>("Operators");

    ret->registerModel<DivisionModel>("Operators");

    ret->registerModel<StringDataModel>("Sources");

    return ret;
}

void WidgetView::setWidget()
{
    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    m_model = new DataFlowGraphModel(registry);

    m_widget_scene = new DataFlowGraphicsScene(*m_model, nullptr);

    m_widget = new GraphicsView(m_widget_scene); // Comp

    updateSizeConstraints();

    QObject::connect(m_widget_scene,
                     &DataFlowGraphicsScene::sceneLoaded,
                     m_widget,
                     &GraphicsView::centerScene);

    QFile file("C:/temp/ttt.flow");

    file.open(QIODevice::ReadOnly);

    m_widget_scene->clearScene();

    QByteArray const wholeFile = file.readAll();

    m_model->load(QJsonDocument::fromJson(wholeFile).object());

    m_widget->centerScene();
}
