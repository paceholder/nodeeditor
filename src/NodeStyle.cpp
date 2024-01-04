#include "NodeStyle.hpp"

#include <iostream>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QtCore/QDebug>

#include "StyleCollection.hpp"

using QtNodes::NodeStyle;

inline void initResources()
{
    Q_INIT_RESOURCE(resources);
}

NodeStyle::NodeStyle()
{
    // Explicit resources inialization for preventing the static initialization
    // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
    initResources();

    // This configuration is stored inside the compiled unit and is loaded statically
    loadJsonFile(":DefaultStyle.json");
}

NodeStyle::NodeStyle(QString jsonText): NodeStyle()
{
    loadJsonText(jsonText);
}

NodeStyle::NodeStyle(QJsonObject const &json): NodeStyle()
{
    loadJson(json);
}

void NodeStyle::setNodeStyle(QString jsonText)
{
    NodeStyle style(jsonText);

    StyleCollection::setNodeStyle(style);
}

void NodeStyle::loadJson(QJsonObject const &json)
{
    QJsonValue nodeStyleValues = json["NodeStyle"];

    QJsonObject obj = nodeStyleValues.toObject();

    X_STYLE_READ_COLOR(obj, NormalBoundaryColor);
    X_STYLE_READ_COLOR(obj, SelectedBoundaryColor);
    X_STYLE_READ_COLOR(obj, GradientColor0);
    X_STYLE_READ_COLOR(obj, GradientColor1);
    X_STYLE_READ_COLOR(obj, GradientColor2);
    X_STYLE_READ_COLOR(obj, GradientColor3);
    X_STYLE_READ_COLOR(obj, ShadowColor);
    X_STYLE_READ_COLOR(obj, FontColor);
    X_STYLE_READ_COLOR(obj, FontColorFaded);
    X_STYLE_READ_COLOR(obj, ConnectionPointColor);
    X_STYLE_READ_COLOR(obj, FilledConnectionPointColor);
    X_STYLE_READ_COLOR(obj, WarningColor);
    X_STYLE_READ_COLOR(obj, ErrorColor);
    X_STYLE_READ_COLOR(obj, FillColor);

    X_STYLE_READ_FLOAT(obj, PenWidth);
    X_STYLE_READ_FLOAT(obj, HoveredPenWidth);
    X_STYLE_READ_FLOAT(obj, ConnectionPointDiameter);

    X_STYLE_READ_FLOAT(obj, Opacity);
    X_STYLE_READ_FLOAT(obj, CornerRadius);
}

QJsonObject NodeStyle::toJson() const
{
    QJsonObject obj;

    X_STYLE_WRITE_COLOR(obj, NormalBoundaryColor);
    X_STYLE_WRITE_COLOR(obj, SelectedBoundaryColor);
    X_STYLE_WRITE_COLOR(obj, GradientColor0);
    X_STYLE_WRITE_COLOR(obj, GradientColor1);
    X_STYLE_WRITE_COLOR(obj, GradientColor2);
    X_STYLE_WRITE_COLOR(obj, GradientColor3);
    X_STYLE_WRITE_COLOR(obj, ShadowColor);
    X_STYLE_WRITE_COLOR(obj, FontColor);
    X_STYLE_WRITE_COLOR(obj, FontColorFaded);
    X_STYLE_WRITE_COLOR(obj, ConnectionPointColor);
    X_STYLE_WRITE_COLOR(obj, FilledConnectionPointColor);
    X_STYLE_WRITE_COLOR(obj, WarningColor);
    X_STYLE_WRITE_COLOR(obj, ErrorColor);
    X_STYLE_WRITE_COLOR(obj, FillColor);

    X_STYLE_WRITE_FLOAT(obj, PenWidth);
    X_STYLE_WRITE_FLOAT(obj, HoveredPenWidth);
    X_STYLE_WRITE_FLOAT(obj, ConnectionPointDiameter);

    X_STYLE_WRITE_FLOAT(obj, Opacity);
    X_STYLE_WRITE_FLOAT(obj, CornerRadius);

    QJsonObject root;
    root["NodeStyle"] = obj;

    return root;
}
