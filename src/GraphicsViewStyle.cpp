#include "GraphicsViewStyle.hpp"

#include "StyleCollection.hpp"

#include <QtCore/QJsonObject>

using QtNodes::GraphicsViewStyle;
using namespace QtNodes::detail;

inline void initResources()
{
    Q_INIT_RESOURCE(resources);
}

GraphicsViewStyle::GraphicsViewStyle()
{
    // Explicit resources inialization for preventing the static initialization
    // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
    initResources();

    // This configuration is stored inside the compiled unit and is loaded statically
    loadJsonFile(":DefaultStyle.json");
}

GraphicsViewStyle::GraphicsViewStyle(QString jsonText)
{
    loadJsonText(jsonText);
}

void GraphicsViewStyle::setStyle(QString jsonText)
{
    GraphicsViewStyle style(jsonText);

    StyleCollection::setGraphicsViewStyle(style);
}

void GraphicsViewStyle::loadJson(QJsonObject const &json)
{
    QJsonObject obj = json["GraphicsViewStyle"].toObject();

    readColor(obj, "BackgroundColor", _BackgroundColor);
    readColor(obj, "FineGridColor", _FineGridColor);
    readColor(obj, "CoarseGridColor", _CoarseGridColor);
}

QJsonObject GraphicsViewStyle::toJson() const
{
    QJsonObject obj;

    writeColor(obj, "BackgroundColor", _BackgroundColor);
    writeColor(obj, "FineGridColor", _FineGridColor);
    writeColor(obj, "CoarseGridColor", _CoarseGridColor);

    QJsonObject root;
    root["GraphicsViewStyle"] = obj;

    return root;
}

QColor GraphicsViewStyle::backgroundColor() const { return _BackgroundColor; }
QColor GraphicsViewStyle::fineGridColor() const { return _FineGridColor; }
QColor GraphicsViewStyle::coarseGridColor() const { return _CoarseGridColor; }
