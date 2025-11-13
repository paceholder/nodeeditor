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

    QIcon statusUpdated{QStringLiteral("://status_icons/updated.svg")};
    QIcon statusProcessing{QStringLiteral("://status_icons/processing.svg")};
    QIcon statusPending{QStringLiteral("://status_icons/pending.svg")};
    QIcon statusInvalid{QStringLiteral("://status_icons/failed.svg")};
    QIcon statusEmpty{QStringLiteral("://status_icons/empty.svg")};
    QIcon statusPartial{QStringLiteral("://status_icons/partial.svg")};

    ProcessingIconStyle processingIconStyle{};
};
} // namespace QtNodes
