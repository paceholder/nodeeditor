#include "StyleImport.hpp"

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValueRef>

#include <QDebug>

using QtNodes::StyleImport;


static
inline
void
initResources()
{
  // note: this function must be in the global namespace.
  // Q_INIT_RESOURCE(...) requires it.
  Q_INIT_RESOURCE(resources);
}


void
StyleImport::
initResources()
{
  ::initResources();
}


QByteArray
StyleImport::
readJsonFile(QString const &fileName)
{
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
  {
    qWarning() << "Couldn't open file " << fileName;

    return QByteArray();
  }

  return file.readAll();
}


QByteArray
StyleImport::
readJsonText(QString const &jsonText)
{
  return jsonText.toUtf8();
}


bool
StyleImport::
hasValue(QJsonValue const &value, QString const &)
{
  return (value.type() != QJsonValue::Undefined
          && value.type() != QJsonValue::Null);
}


void
StyleImport::
checkUndefinedValue(QJsonValue const &value, QString const &name)
{
  if (!hasValue(value, name)) {
    qWarning() << "Undefined value for parameter: " << name;
  }
}


bool
StyleImport::
readColor(QJsonObject const &obj, QString const &name, QColor *output)
{
  auto valueRef = obj[name];

  if (StyleDebug) {
    checkUndefinedValue(valueRef, name);
  }

  if (!hasValue(valueRef, name)) return false;

  if (valueRef.isArray()) {
    auto colors = valueRef.toArray();

    *output = QColor(colors.at(0).toInt(),
                     colors.at(1).toInt(),
                     colors.at(2).toInt());
  }
  else
  {
    *output = QColor(valueRef.toString());
  }

  return true;
}


bool
StyleImport::
readFloat(QJsonObject const &obj, QString const &name, float *output)
{
  auto valueRef = obj[name];

  if (StyleDebug) {
    checkUndefinedValue(valueRef, name);
  }

  if (!hasValue(valueRef, name)) return false;

  *output = valueRef.toDouble();

  return true;
}


bool
StyleImport::
readBool(QJsonObject const &obj, QString const &name, bool *output)
{
  auto valueRef = obj[name];

  if (StyleDebug) {
    checkUndefinedValue(valueRef, name);
  }

  if (!hasValue(valueRef, name)) return false;

  *output = valueRef.toBool();

  return true;
}


QColor
StyleImport::
readColor(QJsonObject const &obj, QString const &name)
{
  QColor result;

  if (!readColor(obj, name, &result)) {
    throw StyleImportError("Missing " + name.toStdString());
  }

  return result;
}


float
StyleImport::
readFloat(QJsonObject const &obj, QString const &name)
{
  float result;

  if (!readFloat(obj, name, &result)) {
    throw StyleImportError("Missing " + name.toStdString());
  }

  return result;
}


bool
StyleImport::
readBool(QJsonObject const &obj, QString const &name)
{
  bool result;

  if (!readBool(obj, name, &result)) {
    throw StyleImportError("Missing " + name.toStdString());
  }

  return result;
}
