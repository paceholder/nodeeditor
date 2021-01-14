#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC FlowViewStyle : public Style
{
public:

  FlowViewStyle();

  FlowViewStyle(const QString& jsonText);

public:

  static void setStyle(const QString& jsonText);

private:

  void loadJsonText(const QString& jsonText) override;

  void loadJsonFile(const QString& fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:

  QColor BackgroundColor;
  QColor FineGridColor;
  QColor CoarseGridColor;
};
}
