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
  loadJsonText(QString jsonText) = 0;

  virtual void
  loadJsonFile(QString fileName) = 0;

  virtual void
  loadJsonFromByteArray(QByteArray const &byteArray) = 0;
};

}
