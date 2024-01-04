#include "ConnectionStyle.hpp"

#include "StyleCollection.hpp"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QDebug>

#include <random>

using QtNodes::ConnectionStyle;

inline void initResources()
{
    Q_INIT_RESOURCE(resources);
}

ConnectionStyle::ConnectionStyle()
{
    // Explicit resources inialization for preventing the static initialization
    // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
    initResources();

    // This configuration is stored inside the compiled unit and is loaded statically
    loadJsonFile(":DefaultStyle.json");
}

ConnectionStyle::ConnectionStyle(QString jsonText): ConnectionStyle()
{
    loadJsonText(jsonText);
}

void ConnectionStyle::setConnectionStyle(QString jsonText)
{
    ConnectionStyle style(jsonText);

    StyleCollection::setConnectionStyle(style);
}

void ConnectionStyle::loadJson(QJsonObject const &json)
{
    QJsonValue nodeStyleValues = json["ConnectionStyle"];

    QJsonObject obj = nodeStyleValues.toObject();

    X_STYLE_READ_COLOR(obj, ConstructionColor);
    X_STYLE_READ_COLOR(obj, NormalColor);
    X_STYLE_READ_COLOR(obj, SelectedColor);
    X_STYLE_READ_COLOR(obj, SelectedHaloColor);
    X_STYLE_READ_COLOR(obj, HoveredColor);

    X_STYLE_READ_FLOAT(obj, LineWidth);
    X_STYLE_READ_FLOAT(obj, ConstructionLineWidth);
    X_STYLE_READ_FLOAT(obj, PointDiameter);

    X_STYLE_READ_BOOL(obj, UseDataDefinedColors);
    X_STYLE_READ_BOOL(obj, WithArrow);
}

QJsonObject ConnectionStyle::toJson() const
{
    QJsonObject obj;

    X_STYLE_WRITE_COLOR(obj, ConstructionColor);
    X_STYLE_WRITE_COLOR(obj, NormalColor);
    X_STYLE_WRITE_COLOR(obj, SelectedColor);
    X_STYLE_WRITE_COLOR(obj, SelectedHaloColor);
    X_STYLE_WRITE_COLOR(obj, HoveredColor);

    X_STYLE_WRITE_FLOAT(obj, LineWidth);
    X_STYLE_WRITE_FLOAT(obj, ConstructionLineWidth);
    X_STYLE_WRITE_FLOAT(obj, PointDiameter);

    X_STYLE_WRITE_BOOL(obj, UseDataDefinedColors);
    X_STYLE_WRITE_BOOL(obj, WithArrow);

    QJsonObject root;
    root["ConnectionStyle"] = obj;

    return root;
}

QColor ConnectionStyle::constructionColor() const
{
    return ConstructionColor;
}

QColor ConnectionStyle::normalColor() const
{
    return NormalColor;
}

QColor ConnectionStyle::normalColor(QString typeId) const
{
    std::size_t hash = qHash(typeId);

    std::size_t const hue_range = 0xFF;

    std::mt19937 gen(static_cast<unsigned int>(hash));
    std::uniform_int_distribution<int> distrib(0, hue_range);

    int hue = distrib(gen);
    int sat = 120 + hash % 129;

    return QColor::fromHsl(hue, sat, 160);
}

QColor ConnectionStyle::selectedColor() const
{
    return SelectedColor;
}

QColor ConnectionStyle::selectedHaloColor() const
{
    return SelectedHaloColor;
}

QColor ConnectionStyle::hoveredColor() const
{
    return HoveredColor;
}

float ConnectionStyle::lineWidth() const
{
    return LineWidth;
}

float ConnectionStyle::constructionLineWidth() const
{
    return ConstructionLineWidth;
}

float ConnectionStyle::pointDiameter() const
{
    return PointDiameter;
}

bool ConnectionStyle::useDataDefinedColors() const
{
    return UseDataDefinedColors;
}

bool ConnectionStyle::withArrow() const
{
    return WithArrow;
}
