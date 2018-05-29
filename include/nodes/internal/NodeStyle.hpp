#pragma once

#include <QtGui/QColor>

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC NodeStyle
{
public:
  NodeStyle();

  static NodeStyle const&
  defaultStyle();

  static NodeStyle
  fromJson(QString const& jsonText);

  QColor normalBoundaryColor() const;
  QColor selectedBoundaryColor() const;
  QColor gradientColor0() const;
  QColor gradientColor1() const;
  QColor gradientColor2() const;
  QColor gradientColor3() const;
  QColor shadowColor() const;

  QColor fontColor() const;
  QColor fontColorFaded() const;

  QColor connectionPointColor() const;
  QColor filledConnectionPointColor() const;

  QColor warningColor() const;
  QColor errorColor() const;

  float penWidth() const;
  float hoveredPenWidth() const;
  float connectionPointDiameter() const;
  float opacity() const;

  void setNormalBoundaryColor(QColor);
  void setSelectedBoundaryColor(QColor);
  void setGradientColor0(QColor);
  void setGradientColor1(QColor);
  void setGradientColor2(QColor);
  void setGradientColor3(QColor);
  void setShadowColor(QColor);

  void setFontColor(QColor);
  void setFontColorFaded(QColor);

  void setConnectionPointColor(QColor);
  void setFilledConnectionPointColor(QColor);

  void setWarningColor(QColor);
  void setErrorColor(QColor);

  void setPenWidth(float);
  void setHoveredPenWidth(float);
  void setConnectionPointDiameter(float);
  void setOpacity(float);

private:

  NodeStyle(QByteArray const& jsonBytes);

private:

  QColor _normalBoundaryColor;
  QColor _selectedBoundaryColor;
  QColor _gradientColor0;
  QColor _gradientColor1;
  QColor _gradientColor2;
  QColor _gradientColor3;
  QColor _shadowColor;
  QColor _fontColor;
  QColor _fontColorFaded;

  QColor _connectionPointColor;
  QColor _filledConnectionPointColor;

  QColor _warningColor;
  QColor _errorColor;

  float _penWidth;
  float _hoveredPenWidth;

  float _connectionPointDiameter;

  float _opacity;
};
}
