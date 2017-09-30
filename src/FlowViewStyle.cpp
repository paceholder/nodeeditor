#include "FlowViewStyle.hpp"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>
#include <QtCore/QJsonArray>

#include <QDebug>

#include "StyleCollection.hpp"

using QtNodes::FlowViewStyle;

inline void initResources() { Q_INIT_RESOURCE(resources); }

FlowViewStyle::
FlowViewStyle()
{
  // Explicit resources inialization for preventing the static initialization
  // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
  initResources();

  // This configuration is stored inside the compiled unit and is loaded statically
  loadJsonFile(":DefaultStyle.json");
}


FlowViewStyle::
FlowViewStyle(QString jsonText)
{
  loadJsonText(jsonText);
}


void
FlowViewStyle::
setStyle(QString jsonText)
{
  FlowViewStyle style(jsonText);

  StyleCollection::setFlowViewStyle(style);
}


#ifdef STYLE_DEBUG
  #define FLOW_VIEW_STYLE_CHECK_UNDEFINED_VALUE(v, variable) { \
      if (v.type() == QJsonValue::Undefined || \
          v.type() == QJsonValue::Null) \
        qWarning() << "Undefined value for parameter:" << #variable; \
  }
#else
  #define FLOW_VIEW_STYLE_CHECK_UNDEFINED_VALUE(v, variable)
#endif

#define FLOW_VIEW_STYLE_READ_COLOR(values, variable)  { \
    auto valueRef = values[#variable]; \
    FLOW_VIEW_STYLE_CHECK_UNDEFINED_VALUE(valueRef, variable) \
    if (valueRef.isArray()) { \
      auto colorArray = valueRef.toArray(); \
      std::vector<int> rgb; rgb.reserve(3); \
      for (auto it = colorArray.begin(); it != colorArray.end(); ++it) { \
        rgb.push_back((*it).toInt()); \
      } \
      variable = QColor(rgb[0], rgb[1], rgb[2]); \
    } else { \
      variable = QColor(valueRef.toString()); \
    } \
}

void
FlowViewStyle::
loadJsonFile(QString styleFile)
{
  QFile file(styleFile);

  if (!file.open(QIODevice::ReadOnly))
  {
    qWarning() << "Couldn't open file " << styleFile;

    return;
  }

  loadJsonFromByteArray(file.readAll());
}


void
FlowViewStyle::
loadJsonText(QString jsonText)
{
  loadJsonFromByteArray(jsonText.toUtf8());
}


void
FlowViewStyle::
loadJsonFromByteArray(QByteArray const &byteArray)
{
  QJsonDocument json(QJsonDocument::fromJson(byteArray));

  QJsonObject topLevelObject = json.object();

  QJsonValueRef nodeStyleValues = topLevelObject["FlowViewStyle"];

  QJsonObject obj = nodeStyleValues.toObject();

  FLOW_VIEW_STYLE_READ_COLOR(obj, BackgroundColor);
  FLOW_VIEW_STYLE_READ_COLOR(obj, FineGridColor);
  FLOW_VIEW_STYLE_READ_COLOR(obj, CoarseGridColor);
}
