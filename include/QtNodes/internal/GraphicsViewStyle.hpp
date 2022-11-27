#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC GraphicsViewStyle : public Style
{
public:

  GraphicsViewStyle();

  GraphicsViewStyle(QString jsonText);

  ~GraphicsViewStyle() = default;

public:

  static void setStyle(QString jsonText);

private:

  void loadJson(QJsonDocument const & json) override;

  QJsonDocument toJson() const override;

public:

  QColor BackgroundColor;
  QColor FineGridColor;
  QColor CoarseGridColor;
};
}
