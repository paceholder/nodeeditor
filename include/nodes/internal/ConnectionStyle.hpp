#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC ConnectionStyle : public Style
{
public:

  ConnectionStyle();

  ConnectionStyle(QString const& jsonText);

public:

  static void setConnectionStyle(QString const& jsonText);

private:

  void loadJsonText(QString const& jsonText) override;

  void loadJsonFile(QString const& fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:

  QColor constructionColor() const;
  QColor normalColor() const;
  QColor normalColor(QString const& typeId) const;
  QColor selectedColor() const;
  QColor selectedHaloColor() const;
  QColor hoveredColor() const;

  float lineWidth() const;
  float constructionLineWidth() const;
  float pointDiameter() const;

  bool useDataDefinedColors() const;

private:

  QColor ConstructionColor;
  QColor NormalColor;
  QColor SelectedColor;
  QColor SelectedHaloColor;
  QColor HoveredColor;

  float LineWidth;
  float ConstructionLineWidth;
  float PointDiameter;

  bool UseDataDefinedColors;
};
}
