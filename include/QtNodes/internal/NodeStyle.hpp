#pragma once

#include "Export.hpp"
#include "Style.hpp"
#include <QColor>
#include <QJsonObject>
#include <QString>

namespace QtNodes {

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
    QColor FontColor;
    QColor FontColorFaded;

    QColor ConnectionPointColor;
    QColor FilledConnectionPointColor;

    QColor WarningColor;
    QColor ErrorColor;

    float PenWidth;
    float HoveredPenWidth;

    float ConnectionPointDiameter;

    float Opacity;
};
} // namespace QtNodes
