#pragma once

#include "Export.hpp"
#include "Style.hpp"
#include <QColor>
#include <QJsonObject>
#include <QString>

namespace QtNodes {

class NODE_EDITOR_PUBLIC GraphicsViewStyle : public Style
{
public:
    GraphicsViewStyle();

    GraphicsViewStyle(QString jsonText);

    ~GraphicsViewStyle() = default;

public:
    static void setStyle(QString jsonText);

private:
    void loadJson(QJsonObject const &json) override;

    QJsonObject toJson() const override;

public:
    QColor BackgroundColor;
    QColor FineGridColor;
    QColor CoarseGridColor;
};
} // namespace QtNodes
