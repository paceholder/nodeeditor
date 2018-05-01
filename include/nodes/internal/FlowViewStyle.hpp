#pragma once

#include <memory>

#include <QtGui/QColor>

#include <QString>
#include <QByteArray>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC FlowViewStyle : public std::enable_shared_from_this<FlowViewStyle>
{
public:
  FlowViewStyle();

  static std::shared_ptr<FlowViewStyle>
  defaultStyle();

  static std::shared_ptr<FlowViewStyle>
  fromJson(QString jsonText);

  QColor const &
  backgroundColor() const;

  QColor const &
  fineGridColor() const;

  QColor const &
  coarseGridColor() const;

  void
  setBackgroundColor(QColor backgroundColor);

  void
  setFineGridColor(QColor fineGridColor);

  void
  setCoarseGridColor(QColor coarseGridColor);

private:

  void loadJson(QByteArray const& jsonBytes);

  QColor _backgroundColor;
  QColor _fineGridColor;
  QColor _coarseGridColor;
};
}
