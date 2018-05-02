#pragma once

#include <memory>

#include <QtGui/QColor>

#include <QByteArray>
#include <QObject>
#include <QString>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC ConnectionStyle
{
public:
  ConnectionStyle();

  static std::shared_ptr<ConnectionStyle>
  defaultStyle();

  static std::shared_ptr<ConnectionStyle>
  fromJson(QString jsonText);

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
  computeNormalColor(QString typeId);

private:
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
