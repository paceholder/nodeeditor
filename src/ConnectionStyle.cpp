#include "ConnectionStyle.hpp"

#include <iostream>
#include <utility>

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QDebug>

#include "StyleImport.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::StyleImport;


void
ConnectionStyle::
loadJson(QByteArray const& jsonBytes)
{
  QJsonDocument json(QJsonDocument::fromJson(jsonBytes));
  QJsonObject topLevelObject = json.object();

  QJsonValueRef nodeStyleValues = topLevelObject["ConnectionStyle"];
  QJsonObject obj = nodeStyleValues.toObject();

  StyleImport::readColor(obj, "ConstructionColor", &_constructionColor);
  StyleImport::readColor(obj, "NormalColor", &_normalColor);
  StyleImport::readColor(obj, "SelectedColor", &_selectedColor);
  StyleImport::readColor(obj, "SelectedHaloColor", &_selectedHaloColor);
  StyleImport::readColor(obj, "HoveredColor", &_hoveredColor);

  StyleImport::readFloat(obj, "LineWidth", &_lineWidth);
  StyleImport::readFloat(obj, "ConstructionLineWidth", &_constructionLineWidth);
  StyleImport::readFloat(obj, "PointDiameter", &_pointDiameter);

  StyleImport::readBool(obj, "UseDataDefinedColors", &_useDataDefinedColors);
}


std::shared_ptr<ConnectionStyle>
ConnectionStyle::
defaultStyle()
{
  StyleImport::initResources();

  auto style = std::make_shared<ConnectionStyle>();

  style->loadJson(StyleImport::readJsonFile(":DefaultStyle.json"));

  return style;
}


std::shared_ptr<ConnectionStyle>
ConnectionStyle::
fromJson(QString jsonText)
{
  auto style = defaultStyle();

  style->loadJson(StyleImport::readJsonText(jsonText));

  return style;
}


// static
QColor
ConnectionStyle::
computeNormalColor(QString typeId)
{
  std::size_t hash = qHash(typeId);

  std::size_t const hue_range = 0xFF;

  qsrand(hash);
  std::size_t hue = qrand() % hue_range;

  std::size_t sat = 120 + hash % 129;

  return QColor::fromHsl(hue,
                         sat,
                         160);
}


ConnectionStyle::
ConnectionStyle()
= default;


QColor
ConnectionStyle::
constructionColor() const
{
  return _constructionColor;
}


QColor
ConnectionStyle::
normalColor() const
{
  return _normalColor;
}


QColor
ConnectionStyle::
selectedColor() const
{
  return _selectedColor;
}


QColor
ConnectionStyle::
selectedHaloColor() const
{
  return _selectedHaloColor;
}


QColor
ConnectionStyle::
hoveredColor() const
{
  return _hoveredColor;
}


float
ConnectionStyle::
lineWidth() const
{
  return _lineWidth;
}


float
ConnectionStyle::
constructionLineWidth() const
{
  return _constructionLineWidth;
}


float
ConnectionStyle::
pointDiameter() const
{
  return _pointDiameter;
}


bool
ConnectionStyle::
useDataDefinedColors() const
{
  return _useDataDefinedColors;
}


void
ConnectionStyle::
setConstructionColor(QColor color)
{
  _constructionColor = std::move(color);
}


void
ConnectionStyle::
setNormalColor(QColor color)
{
  _normalColor = std::move(color);
}


void
ConnectionStyle::
setSelectedColor(QColor color)
{
  _selectedColor = std::move(color);
}


void
ConnectionStyle::
setSelectedHaloColor(QColor color)
{
  _selectedHaloColor = std::move(color);
}


void
ConnectionStyle::
setHoveredColor(QColor color)
{
  _hoveredColor = std::move(color);
}


void
ConnectionStyle::
setLineWidth(float value)
{
  _lineWidth = value;
}


void
ConnectionStyle::
setConstructionLineWidth(float value)
{
  _constructionLineWidth = value;
}


void
ConnectionStyle::
setPointDiameter(float value)
{
  _pointDiameter = value;
}


void
ConnectionStyle::
useDataDefinedColors(bool use)
{
  _useDataDefinedColors = use;
}
