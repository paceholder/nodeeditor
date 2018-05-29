#pragma once

#include <QtGui/QColor>

#include <QtCore/QString>
#include <QtCore/QByteArray>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC FlowViewStyle
{
public:
  FlowViewStyle();

  static FlowViewStyle const&
  defaultStyle();

  static FlowViewStyle
  fromJson(QString const& jsonText);

  QColor const &
  backgroundColor() const
  {
    return _backgroundColor;
  }

  QColor const &
  fineGridColor() const
  {
    return _fineGridColor;
  }

  QColor const &
  coarseGridColor() const
  {
    return _coarseGridColor;
  }

  void
  setBackgroundColor(QColor backgroundColor)
  {
    _backgroundColor = backgroundColor;
  }

  void
  setFineGridColor(QColor fineGridColor)
  {
    _fineGridColor = fineGridColor;
  }

  void
  setCoarseGridColor(QColor coarseGridColor)
  {
    _coarseGridColor = coarseGridColor;
  }

private:
  FlowViewStyle(QByteArray const& jsonBytes);

  QColor _backgroundColor;
  QColor _fineGridColor;
  QColor _coarseGridColor;
};
}
