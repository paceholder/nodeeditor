#pragma once

#include <QtCore/QString>

namespace QtNodes
{

class Style
{
public:

  virtual
  ~Style() = default;

private:

  virtual void
  loadJsonText(const QString& jsonText) = 0;

  virtual void
  loadJsonFile(const QString& fileName) = 0;

  virtual void
  loadJsonFromByteArray(QByteArray const &byteArray) = 0;
};

}
