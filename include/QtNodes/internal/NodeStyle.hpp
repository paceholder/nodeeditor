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
    QColor NormalBoundaryColor;
    QColor SelectedBoundaryColor;
    QColor GradientColor0;
    QColor GradientColor1;
    QColor GradientColor2;
    QColor GradientColor3;
    QColor ShadowColor;
    bool ShadowEnabled;
    QColor FontColor;
    QColor FontColorFaded;

    QColor ConnectionPointColor;
    QColor FilledConnectionPointColor;

    QColor WarningColor;
    QColor ErrorColor;
    QColor ToolTipIconColor;

    float PenWidth;
    float HoveredPenWidth;

    float ConnectionPointDiameter;

    float Opacity;

    // Status icons - initialized in constructor after Q_INIT_RESOURCE
    QIcon statusUpdated;
    QIcon statusProcessing;
    QIcon statusPending;
    QIcon statusInvalid;
    QIcon statusEmpty;
    QIcon statusPartial;

    ProcessingIconStyle processingIconStyle{};
};
} // namespace QtNodes
