#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

class NODE_EDITOR_PUBLIC ConnectionStyle : public Style
{
public:

  ConnectionStyle();

  ConnectionStyle(QString jsonText);

public:

  static void
  setConnectionStyle(QString jsonText);

private:

  void
  loadJsonText(QString jsonText) override;

  void
  loadJsonFile(QString fileName) override;

  void
  loadJsonFromByteArray(QByteArray const &byteArray) override;

public:

  QColor ConstructionColor;
  QColor NormalColor;
  QColor SelectedColor;
  QColor SelectedHaloColor;
  QColor HoveredColor;

  float LineWidth;
  float ConstructionLineWidth;
  float PointDiameter;
};
