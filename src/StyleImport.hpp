#pragma once

#include <stdexcept>

#include <QtGui/QColor>

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

namespace QtNodes
{

class StyleImportError : public std::logic_error
{
public:
  using std::logic_error::logic_error;
};

class StyleImport
{
public:
#ifdef STYLE_DEBUG
  static constexpr bool StyleDebug = true;
#else
  static constexpr bool StyleDebug = false;
#endif

  static void
  initResources();

  static QByteArray
  readJsonFile(QString const &fileName);

  static QByteArray
  readJsonText(QString const &jsonText);

  static bool
  hasValue(QJsonValue const &value, QString const &name);

  static void
  checkUndefinedValue(QJsonValue const &vlaue, QString const &name);

  static bool
  readColor(QJsonObject const &obj, QString const &name, QColor *output);

  static bool
  readFloat(QJsonObject const &obj, QString const &name, float *output);

  static bool
  readBool(QJsonObject const &obj, QString const &name, bool *output);

  static QColor
  readColor(QJsonObject const &obj, QString const &name);

  static float
  readFloat(QJsonObject const &obj, QString const &name);

  static bool
  readBool(QJsonObject const &obj, QString const &name);
};
}
