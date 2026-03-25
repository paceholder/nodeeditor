#pragma once

#include <QIcon>
#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes {

/**
    * Describes the position of the processing icon on the node ui
     */
enum class ProcessingIconPos {
    BottomLeft = 0,  /// icon on the bottom left position
    BottomRight = 1, /// icon on the bottom right position
};

/**
 * Defines the processing icon style;
 */
struct ProcessingIconStyle
{
    ProcessingIconPos _pos{ProcessingIconPos::BottomRight};
    double _size{20.0};
    double _margin{8.0};
    int _resolution{64};
};

class NODE_EDITOR_PUBLIC NodeStyle : public Style
{
public:
    NodeStyle();

    NodeStyle(QString jsonText);

    NodeStyle(QJsonObject const &json);

    virtual ~NodeStyle() = default;

public:
    static void setNodeStyle(QString jsonText);

public:
    void loadJson(QJsonObject const &json) override;

    QJsonObject toJson() const override;

    /// Set uniform background color for the node.
    void setBackgroundColor(QColor const &color);

    /// Current uniform background color.
    QColor backgroundColor() const;

public:
    QColor normalBoundaryColor() const;
    QColor selectedBoundaryColor() const;
    QColor gradientColor0() const;
    QColor gradientColor1() const;
    QColor gradientColor2() const;
    QColor gradientColor3() const;
    QColor shadowColor() const;
    bool shadowEnabled() const;
    QColor fontColor() const;
    QColor fontColorFaded() const;
    QColor connectionPointColor() const;
    QColor filledConnectionPointColor() const;
    QColor warningColor() const;
    QColor errorColor() const;
    QColor toolTipIconColor() const;
    float penWidth() const;
    float hoveredPenWidth() const;
    float connectionPointDiameter() const;
    float opacity() const;
    QIcon const &statusUpdated() const;
    QIcon const &statusProcessing() const;
    QIcon const &statusPending() const;
    QIcon const &statusInvalid() const;
    QIcon const &statusEmpty() const;
    QIcon const &statusPartial() const;
    ProcessingIconStyle const &processingIconStyle() const;

    void setStatusUpdated(QIcon const &icon);
    void setStatusProcessing(QIcon const &icon);
    void setStatusPending(QIcon const &icon);
    void setStatusInvalid(QIcon const &icon);
    void setStatusEmpty(QIcon const &icon);
    void setStatusPartial(QIcon const &icon);
    void setProcessingIconStyle(ProcessingIconStyle const &style);

private:
    QColor _NormalBoundaryColor;
    QColor _SelectedBoundaryColor;
    QColor _GradientColor0;
    QColor _GradientColor1;
    QColor _GradientColor2;
    QColor _GradientColor3;
    QColor _ShadowColor;
    bool _ShadowEnabled{false};
    QColor _FontColor;
    QColor _FontColorFaded;

    QColor _ConnectionPointColor;
    QColor _FilledConnectionPointColor;

    QColor _WarningColor;
    QColor _ErrorColor;
    QColor _ToolTipIconColor;

    float _PenWidth{0.0f};
    float _HoveredPenWidth{0.0f};

    float _ConnectionPointDiameter{0.0f};

    float _Opacity{0.0f};

    QIcon _statusUpdated;
    QIcon _statusProcessing;
    QIcon _statusPending;
    QIcon _statusInvalid;
    QIcon _statusEmpty;
    QIcon _statusPartial;

    ProcessingIconStyle _processingIconStyle{};
};
} // namespace QtNodes
