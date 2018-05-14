#pragma once

#include <QtGui/QColor>

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC ConnectionStyle
{
public:
  ConnectionStyle();

  static ConnectionStyle const&
  defaultStyle();

  static ConnectionStyle
  fromJson(QString const& jsonText);

  QColor constructionColor() const;
  QColor normalColor() const;
  QColor selectedColor() const;
  QColor selectedHaloColor() const;
  QColor hoveredColor() const;

  float lineWidth() const;
  float constructionLineWidth() const;
  float pointDiameter() const;

  bool useDataDefinedColors() const;

  void setConstructionColor(QColor);
  void setNormalColor(QColor);
  void setSelectedColor(QColor);
  void setSelectedHaloColor(QColor);
  void setHoveredColor(QColor);

  void setLineWidth(float);
  void setConstructionLineWidth(float);
  void setPointDiameter(float);

  void useDataDefinedColors(bool);

  static QColor
  computeNormalColor(QString const& typeId);

private:
  ConnectionStyle(QByteArray const& jsonBytes);
  void loadJson(QByteArray const& jsonBytes);

  QColor _constructionColor;
  QColor _normalColor;
  QColor _selectedColor;
  QColor _selectedHaloColor;
  QColor _hoveredColor;

  float _lineWidth;
  float _constructionLineWidth;
  float _pointDiameter;

  bool _useDataDefinedColors;
};
}
