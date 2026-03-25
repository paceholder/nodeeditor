#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

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
    QColor backgroundColor() const;
    QColor fineGridColor() const;
    QColor coarseGridColor() const;

private:
    QColor _BackgroundColor;
    QColor _FineGridColor;
    QColor _CoarseGridColor;
};
} // namespace QtNodes
