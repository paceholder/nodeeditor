#include "FlowViewStyle.hpp"

#include <utility>

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QDebug>

#include "StyleImport.hpp"

using QtNodes::FlowViewStyle;
using QtNodes::StyleImport;
using QtNodes::StyleImportError;


void
FlowViewStyle::
loadJson(QByteArray const &jsonBytes)
{
  QJsonDocument json(QJsonDocument::fromJson(jsonBytes));
  QJsonObject   topLevelObject = json.object();

  QJsonValueRef flowViewStyleValues = topLevelObject["FlowViewStyle"];
  QJsonObject   obj                 = flowViewStyleValues.toObject();

  _backgroundColor = StyleImport::readColor(obj, "BackgroundColor");
  _fineGridColor   = StyleImport::readColor(obj, "FineGridColor");
  _coarseGridColor = StyleImport::readColor(obj, "CoarseGridColor");
}


std::shared_ptr<FlowViewStyle>
FlowViewStyle::
defaultStyle()
{
  StyleImport::initResources();

  auto style = std::make_shared<FlowViewStyle>();
  style->loadJson(StyleImport::readJsonFile(":DefaultStyle.json"));

  return style;
}


std::shared_ptr<FlowViewStyle>
FlowViewStyle::
fromJson(QString jsonText)
{
  auto style = std::make_shared<FlowViewStyle>();
  style->loadJson(StyleImport::readJsonText(jsonText));

  return style;
}


FlowViewStyle::
FlowViewStyle()
= default;
