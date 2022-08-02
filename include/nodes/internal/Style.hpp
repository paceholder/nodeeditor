#pragma once

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QObject>
#include <QtCore/QString>

namespace QtNodes
{

class Style // : public QObject
{
  //Q_OBJECT

public:

  virtual ~Style() = default;

public:

  virtual
  void loadJson(QJsonDocument const & json) = 0;

  virtual
  QJsonDocument toJson() const = 0;

  /// Loads from utf-8 byte array.
  virtual
  void loadJsonFromByteArray(QByteArray const & byteArray)
  {
    QJsonDocument json(QJsonDocument::fromJson(byteArray));

    loadJson(json);
  }

  virtual
  void loadJsonText(QString jsonText)
  {
    loadJsonFromByteArray(jsonText.toUtf8());
  }

  virtual
  void loadJsonFile(QString fileName)
  {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << "Couldn't open file " << fileName;

      return;
    }

    loadJsonFromByteArray(file.readAll());
  }

};

}
