#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC NodeStyle : public Style
{
public:

  NodeStyle();

  NodeStyle(QString jsonText);

public:

  static void setNodeStyle(QString jsonText);

private:

  void loadJsonText(QString jsonText) override;

  void loadJsonFile(QString fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:

  QColor NormalBoundaryColor;
  QColor SelectedBoundaryColor;
  QColor GradientColor0;
  QColor GradientColor1;
  QColor GradientColor2;
  QColor GradientColor3;
  QColor SelectedGradientColor0;
  QColor SelectedGradientColor1;
  QColor SelectedGradientColor2;
  QColor SelectedGradientColor3;  
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
}
