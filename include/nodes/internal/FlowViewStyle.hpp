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

  FlowViewStyle(QString const& jsonText);

public:

  static void setStyle(QString const& jsonText);

private:

  void loadJsonText(QString const& jsonText) override;

  void loadJsonFile(QString const& fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:

  QColor BackgroundColor;
  QColor FineGridColor;
  QColor CoarseGridColor;
};
}
