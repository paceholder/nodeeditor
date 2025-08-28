#pragma once

#include "BasicGraphicsScene.hpp"
#include "Definitions.hpp"
#include "NodeGroup.hpp"
#include <QPen>
#include <QtWidgets/QGraphicsRectItem>

/**
 * @brief The IconGraphicsItem class is an auxiliary class that implements
 * custom behaviour to a fixed-size icon object.
 */
class IconGraphicsItem : public QGraphicsPixmapItem
{
public:
    IconGraphicsItem(QGraphicsItem *parent = nullptr);

    IconGraphicsItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

    /**
   * @brief Returns the factor by which the original image was scaled
   * to fit the desired icon size.
   */
    double scaleFactor() const;

    /**
   * @brief Returns the icon size.
   */
    static constexpr double iconSize() { return _iconSize; }

private:
    double _scaleFactor{};

private:
    static constexpr double _iconSize = 24.0;
};

namespace QtNodes {

class BasicGraphicsScene;
class NodeGroup;
class NodeGraphicsObject;

/**
 * @brief The GroupGraphicsObject class handles the graphical part of a node group.
 * Each node group is associated with a unique GroupGraphicsObject.
 */
class GroupGraphicsObject
    : public QObject
    , public QGraphicsRectItem
{
    Q_OBJECT

public:
    /**
   * @brief Constructor that creates a group's graphical object that should be
   * included in the given scene and associated with the given NodeGroup object.
   * @param scene Reference to the scene that will include this object.
   * @param nodeGroup Reference to the group associated with this object.
   */
    GroupGraphicsObject(BasicGraphicsScene &scene, NodeGroup &nodeGroup);

    GroupGraphicsObject(const GroupGraphicsObject &ggo) = delete;
    GroupGraphicsObject &operator=(const GroupGraphicsObject &other) = delete;
    GroupGraphicsObject(GroupGraphicsObject &&ggo) = delete;
    GroupGraphicsObject &operator=(GroupGraphicsObject &&other) = delete;

    ~GroupGraphicsObject() override;

    /**
   * @brief Returns a reference to this object's associated group.
   */
    NodeGroup &group();

    /**
   * @brief Returns a const reference to this object's associated group.
   */
    NodeGroup const &group() const;

    /**
   * @copydoc QGraphicsItem::boundingRect()
   */
    QRectF boundingRect() const override;

    enum { Type = UserType + 3 };

    /**
   * @copydoc QGraphicsItem::type()
   */
    int type() const override { return Type; }

    /**
   * @brief Sets the group's area color.
   * @param color Color to paint the group area.
   */
    void setFillColor(const QColor &color);

    /**
   * @brief Sets the group's border color.
   * @param color Color to paint the group's border.
   */
    void setBorderColor(const QColor &color);

    /**
   * @brief Updates the position of all the connections that are incident
   * to the nodes of this group.
   */
    void moveConnections();

    /**
   * @brief Moves the position of all the nodes of this group by the amount given.
   * @param offset 2D vector representing the amount by which the group has moved.
   */
    void moveNodes(const QPointF &offset);

    /**
   * @brief Sets the lock state of the group. Locked groups don't allow individual
   * interactions with its nodes, and can only be moved or selected as a whole.
   * @param locked Determines whether this group should be locked.
   */
    void lock(bool locked);

    /**
   * @brief Returns the lock state of the group. Locked groups don't allow individual
   * interactions with its nodes, and can only be moved or selected as a whole.
   */
    bool locked() const;

    /**
   * @brief Updates the position of the group's padlock icon to
   * the top-right corner.
   */
    void positionLockedIcon();

    /**
   * @brief Sets the group hovered state. When the mouse pointer hovers over
   * (or leaves) a group, the group's appearance changes.
   * @param hovered Determines the hovered state.
   */
    void setHovered(bool hovered);

    /**
   * @brief When a node is dragged within the borders of a group, the group's
   * area expands to include the node until the node leaves the area or is
   * released in the group. This function temporarily sets the node as the
   * possible newest member of the group, making the group's area expand.
   * @param possibleChild Pointer to the node that may be included.
   */
    void setPossibleChild(NodeGraphicsObject *possibleChild);

    /**
   * @brief Clears the possibleChild variable.
   * @note See setPossibleChild(NodeGraphicsObject*).
   */
    void unsetPossibleChild();

    /**
   * @brief Returns all the connections that are incident strictly within the
   * nodes of this group.
   */
    std::vector<std::shared_ptr<ConnectionId>> connections() const;

    /**
   * @brief Sets the position of the group.
   * @param position The desired (top-left corner) position of the group, in
   * scene coordinates.
   */
    void setPosition(const QPointF &position);

protected:
    /** @copydoc QGraphicsItem::paint() */
    void paint(QPainter *painter,
               QStyleOptionGraphicsItem const *option,
               QWidget *widget = nullptr) override;

    /** @copydoc QGraphicsItem::hoverEnterEvent() */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /** @copydoc QGraphicsItem::hoverLeaveEvent() */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /** @copydoc QGraphicsItem::mouseMoveEvent() */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /** @copydoc QGraphicsItem::mouseDoubleClickEvent() */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

public:
    /**
   * @brief _currentFillColor Holds the current color of the group background.
   */
    QColor _currentFillColor;

    /**
   * @brief _currentBorderColor Holds the current color of the group border.
   */
    QColor _currentBorderColor;

    const QColor kUnlockedFillColor = QColor("#20a5b084");
    const QColor kUnlockedHoverColor = QColor("#2083a4af");

    const QColor kLockedFillColor = QColor("#3fe0bebc");
    const QColor kLockedHoverColor = QColor("#3feecdcb");

    const QColor kSelectedBorderColor = QColor("#eeffa500");
    const QColor kUnselectedBorderColor = QColor("#eeaaaaaa");

    /**
   * @brief _borderPen Object that dictates how the group border should be drawn.
   */
    QPen _borderPen;

private:
    /**
   * @brief _scene Reference to the scene object in which this object is included.
   */
    BasicGraphicsScene &_scene;

    /**
   * @brief _group Reference to the group instance that corresponds to this object.
   */
    NodeGroup &_group;

    IconGraphicsItem *_lockedGraphicsItem;
    IconGraphicsItem *_unlockedGraphicsItem;

    QPixmap _lockedIcon{QStringLiteral("://padlock-lock.png")};
    QPixmap _unlockedIcon{QStringLiteral("://padlock-unlock.png")};

    /**
   * @brief _possibleChild Pointer that temporarily is set to an existing node when
   * the user drags the node to this group's area.
   */
    NodeGraphicsObject *_possibleChild;

    /**
   * @brief _locked Holds the lock state of the group. Locked groups don't allow individual
   * interactions with its nodes, and can only be moved or selected as a whole.
   */
    bool _locked;

    static constexpr double _groupBorderX = 25.0;
    static constexpr double _groupBorderY = _groupBorderX * 0.8;
    static constexpr double _roundedBorderRadius = _groupBorderY;
    static constexpr QMarginsF _margins = QMarginsF(_groupBorderX,
                                                    _groupBorderY + IconGraphicsItem::iconSize(),
                                                    _groupBorderX + IconGraphicsItem::iconSize(),
                                                    _groupBorderY);

    static constexpr double _defaultWidth = 50.0;
    static constexpr double _defaultHeight = 50.0;

    static constexpr double _groupAreaZValue = 2.0;
};

} // namespace QtNodes
