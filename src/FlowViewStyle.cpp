#include "FlowViewStyle.hpp"

#include <utility>

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include "StyleImport.hpp"

using QtNodes::FlowViewStyle;
using QtNodes::StyleImport;


FlowViewStyle::
FlowViewStyle(QByteArray const& jsonBytes)
{
  QJsonDocument json(QJsonDocument::fromJson(jsonBytes));
  QJsonObject   topLevelObject = json.object();

  QJsonValueRef flowViewStyleValues = topLevelObject["FlowViewStyle"];
  QJsonObject   obj                 = flowViewStyleValues.toObject();

  _backgroundColor = StyleImport::readColor(obj, "BackgroundColor");
  _fineGridColor   = StyleImport::readColor(obj, "FineGridColor");
  _coarseGridColor = StyleImport::readColor(obj, "CoarseGridColor");
}


FlowViewStyle const&
FlowViewStyle::
defaultStyle()
{
  static FlowViewStyle const DefaultStyle = [] {
    StyleImport::initResources();
    return FlowViewStyle(StyleImport::readJsonFile(":DefaultStyle.json"));
  }();

  return DefaultStyle;
}


FlowViewStyle
FlowViewStyle::
fromJson(QString const& jsonText)
{
  return FlowViewStyle(StyleImport::readJsonText(jsonText));
}


FlowViewStyle::
FlowViewStyle()
= default;
