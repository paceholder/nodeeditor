#include "GroupGraphicsObject.hpp"
#include "NodeState.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QtNodes/DataFlowGraphModel>

#include "BasicGraphicsScene.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeGroup;
using QtNodes::DataFlowGraphModel;
using QtNodes::BasicGraphicsScene;
using QtNodes::NodeState;


IconGraphicsItem::
    IconGraphicsItem(QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent) {}


IconGraphicsItem::
    IconGraphicsItem(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    _scaleFactor = _iconSize / pixmap.size().width();
    setScale(_scaleFactor);
}


double
    IconGraphicsItem::
    scaleFactor() const
{
    return _scaleFactor;
}



GroupGraphicsObject::
    GroupGraphicsObject(BasicGraphicsScene& scene,
                        NodeGroup& nodeGroup)
    : _scene(scene)
    , _group(nodeGroup)
    , _possibleChild(nullptr)
    , _locked(false)
{
    setRect(0, 0, _defaultWidth, _defaultHeight);

    _lockedGraphicsItem = new IconGraphicsItem(_lockedIcon, this);
    _unlockedGraphicsItem = new IconGraphicsItem(_unlockedIcon, this);

    _scene.addItem(this);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

    _currentFillColor = kUnlockedFillColor;
    _currentBorderColor = kUnselectedBorderColor;

    _borderPen = QPen(_currentBorderColor, 1.0, Qt::PenStyle::DashLine);

    setZValue(-_groupAreaZValue);

    setAcceptHoverEvents(true);
}


GroupGraphicsObject::
    ~GroupGraphicsObject()
{
    _scene.removeItem(this);
}

NodeGroup&
    GroupGraphicsObject::
    group()
{
    return _group;
}


NodeGroup const&
    GroupGraphicsObject::
    group() const
{
    return _group;
}


QRectF
    GroupGraphicsObject::
    boundingRect() const
{
    QRectF ret{};
    for (auto& node : _group.childNodes())
    {
        NodeGraphicsObject* ngo = node->nodeGraphicsObject();

        ret |= ngo->mapRectToScene(ngo->boundingRect());
    }
    if (_possibleChild)
    {
        ret |= _possibleChild->mapRectToScene(_possibleChild->boundingRect());
    }
    return mapRectFromScene(ret.marginsAdded(_margins));
}

void
    GroupGraphicsObject::
    setFillColor(const QColor& color)
{
    _currentFillColor = color;
    update();
}

void
    GroupGraphicsObject::
    setBorderColor(const QColor& color)
{
    _currentBorderColor = color;
    _borderPen.setColor(_currentBorderColor);
}


void
    GroupGraphicsObject::
    moveConnections()
{
    for (auto& node : group().childNodes())
    {
        node->nodeGraphicsObject().moveConnections();
    }
}


void
    GroupGraphicsObject::
    moveNodes(const QPointF& offset)
{
    for (auto& node : group().childNodes())
    {
        node->nodeGraphicsObject().moveBy(offset.x(), offset.y());
    }
}


void
    GroupGraphicsObject::
    lock(bool locked)
{
    for (auto& node : _group.childNodes())
    {
        node->nodeGraphicsObject().lock(locked);
    }
    _lockedGraphicsItem->setVisible(locked);
    _unlockedGraphicsItem->setVisible(!locked);
    setFillColor(locked? kLockedFillColor : kUnlockedFillColor);
    _locked = locked;
    setZValue(locked? _groupAreaZValue : -_groupAreaZValue);
}


bool
    GroupGraphicsObject::
    locked() const
{
    return _locked;
}


void
    GroupGraphicsObject::
    positionLockedIcon()
{
    _lockedGraphicsItem->setPos(boundingRect().topRight()
                                + QPointF(-(_roundedBorderRadius
                                           + IconGraphicsItem::iconSize()),
                                         _roundedBorderRadius));
    _unlockedGraphicsItem->setPos(boundingRect().topRight()
                                  + QPointF(-(_roundedBorderRadius
                                              + IconGraphicsItem::iconSize()),
                                            _roundedBorderRadius));
}


void
    GroupGraphicsObject::
    setHovered(bool hovered)
{
    hovered?
        setFillColor(locked()? kLockedHoverColor : kUnlockedHoverColor) :
        setFillColor(locked()? kLockedFillColor : kUnlockedFillColor);

    for (auto& node : _group.childNodes())
    {
        node->nodeGeometry().setHovered(hovered);
        node->nodeGraphicsObject().update();
    }
    update();
}


void
    GroupGraphicsObject::
    setPossibleChild(QtNodes::NodeGraphicsObject* possibleChild)
{
    _possibleChild = possibleChild;
}


void
    GroupGraphicsObject::
    unsetPossibleChild()
{
    _possibleChild = nullptr;
}


std::vector<std::shared_ptr<Connection> >
    GroupGraphicsObject::
    connections() const
{
    return _scene.connectionsWithinGroup(group().id());
}


void
    GroupGraphicsObject::
    setPosition(const QPointF& position)
{
    QPointF diffPos = position - scenePos();
    moveNodes(diffPos);
    moveConnections();
}


void
    GroupGraphicsObject::
    hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    setHovered(true);
}

void
    GroupGraphicsObject::
    hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    setHovered(false);
}

void
    GroupGraphicsObject::
    mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
    if (event->lastPos() != event->pos())
    {
        auto diff = event->pos() - event->lastPos();
        moveNodes(diff);
        moveConnections();
    }
}

void
    GroupGraphicsObject::
    mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    lock(!locked());
}


void
    GroupGraphicsObject::
    paint(QPainter* painter,
          const QStyleOptionGraphicsItem* option,
          QWidget* widget)
{
    Q_UNUSED(widget);
    prepareGeometryChange();
    setRect(boundingRect());
    positionLockedIcon();
    painter->setClipRect(option->exposedRect);
    painter->setBrush(_currentFillColor);

    setBorderColor(isSelected()? kSelectedBorderColor : kUnselectedBorderColor);
    painter->setPen(_borderPen);

    painter->drawRoundedRect(rect(), _roundedBorderRadius, _roundedBorderRadius);
}
