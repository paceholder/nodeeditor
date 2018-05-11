#include "NodeStyle.hpp"

#include <iostream>
#include <utility>

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QDebug>

#include "StyleImport.hpp"

using QtNodes::NodeStyle;
using QtNodes::StyleImport;


NodeStyle::
NodeStyle(QByteArray const& jsonBytes)
{
  QJsonDocument json(QJsonDocument::fromJson(jsonBytes));
  QJsonObject   topLevelObject = json.object();

  QJsonValueRef nodeStyleValues = topLevelObject["NodeStyle"];
  QJsonObject   obj             = nodeStyleValues.toObject();

  _normalBoundaryColor        = StyleImport::readColor(obj, "NormalBoundaryColor");
  _selectedBoundaryColor      = StyleImport::readColor(obj, "SelectedBoundaryColor");
  _gradientColor0             = StyleImport::readColor(obj, "GradientColor0");
  _gradientColor1             = StyleImport::readColor(obj, "GradientColor1");
  _gradientColor2             = StyleImport::readColor(obj, "GradientColor2");
  _gradientColor3             = StyleImport::readColor(obj, "GradientColor3");
  _shadowColor                = StyleImport::readColor(obj, "ShadowColor");
  _fontColor                  = StyleImport::readColor(obj, "FontColor");
  _fontColorFaded             = StyleImport::readColor(obj, "FontColorFaded");
  _connectionPointColor       = StyleImport::readColor(obj, "ConnectionPointColor");
  _filledConnectionPointColor = StyleImport::readColor(obj, "FilledConnectionPointColor");
  _warningColor               = StyleImport::readColor(obj, "WarningColor");
  _errorColor                 = StyleImport::readColor(obj, "ErrorColor");

  _penWidth                = StyleImport::readFloat(obj, "PenWidth");
  _hoveredPenWidth         = StyleImport::readFloat(obj, "HoveredPenWidth");
  _connectionPointDiameter = StyleImport::readFloat(obj, "ConnectionPointDiameter");

  _opacity = StyleImport::readFloat(obj, "Opacity");
}


NodeStyle const&
NodeStyle::
defaultStyle()
{
  static NodeStyle const DefaultStyle = [] {
    StyleImport::initResources();

    return NodeStyle(StyleImport::readJsonFile(":DefaultStyle.json"));
  }();

  return DefaultStyle;
}


NodeStyle
NodeStyle::
fromJson(QString const& jsonText)
{
  return NodeStyle(StyleImport::readJsonText(jsonText));
}


NodeStyle::
NodeStyle()
= default;


QColor
NodeStyle::
normalBoundaryColor() const
{
  return _normalBoundaryColor;
}


QColor
NodeStyle::
selectedBoundaryColor() const
{
  return _selectedBoundaryColor;
}


QColor
NodeStyle::
gradientColor0() const
{
  return _gradientColor0;
}


QColor
NodeStyle::
gradientColor1() const
{
  return _gradientColor1;
}


QColor
NodeStyle::
gradientColor2() const
{
  return _gradientColor2;
}


QColor
NodeStyle::
gradientColor3() const
{
  return _gradientColor3;
}


QColor
NodeStyle::
shadowColor() const
{
  return _shadowColor;
}


QColor
NodeStyle::
fontColor() const
{
  return _fontColor;
}


QColor
NodeStyle::
fontColorFaded() const
{
  return _fontColorFaded;
}


QColor
NodeStyle::
connectionPointColor() const
{
  return _connectionPointColor;
}


QColor
NodeStyle::
filledConnectionPointColor() const
{
  return _filledConnectionPointColor;
}


QColor
NodeStyle::
warningColor() const
{
  return _warningColor;
}


QColor
NodeStyle::
errorColor() const
{
  return _errorColor;
}


float
NodeStyle::
penWidth() const
{
  return _penWidth;
}


float
NodeStyle::
hoveredPenWidth() const
{
  return _hoveredPenWidth;
}


float
NodeStyle::
connectionPointDiameter() const
{
  return _connectionPointDiameter;
}


float
NodeStyle::
opacity() const
{
  return _opacity;
}

void
NodeStyle::
setNormalBoundaryColor(QColor color)
{
  _normalBoundaryColor = std::move(color);
}


void
NodeStyle::
setSelectedBoundaryColor(QColor color)
{
  _selectedBoundaryColor = std::move(color);
}


void
NodeStyle::
setGradientColor0(QColor color)
{
  _gradientColor0 = std::move(color);
}


void
NodeStyle::
setGradientColor1(QColor color)
{
  _gradientColor1 = std::move(color);
}


void
NodeStyle::
setGradientColor2(QColor color)
{
  _gradientColor2 = std::move(color);
}


void
NodeStyle::
setGradientColor3(QColor color)
{
  _gradientColor3 = std::move(color);
}


void
NodeStyle::
setShadowColor(QColor color)
{
  _shadowColor = std::move(color);
}


void
NodeStyle::
setFontColor(QColor color)
{
  _fontColor = std::move(color);
}


void
NodeStyle::
setFontColorFaded(QColor color)
{
  _fontColorFaded = std::move(color);
}


void
NodeStyle::
setConnectionPointColor(QColor color)
{
  _connectionPointColor = std::move(color);
}


void
NodeStyle::
setFilledConnectionPointColor(QColor color)
{
  _filledConnectionPointColor = std::move(color);
}


void
NodeStyle::
setWarningColor(QColor color)
{
  _warningColor = std::move(color);
}


void
NodeStyle::
setErrorColor(QColor color)
{
  _errorColor = std::move(color);
}

void
NodeStyle::
setPenWidth(float value)
{
  _penWidth = value;
}


void
NodeStyle::
setHoveredPenWidth(float value)
{
  _hoveredPenWidth = value;
}


void
NodeStyle::
setConnectionPointDiameter(float value)
{
  _connectionPointDiameter = value;
}


void
NodeStyle::
setOpacity(float value)
{
  _opacity = value;
}
