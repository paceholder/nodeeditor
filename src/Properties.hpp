#pragma once

#include <unordered_map>

#include <QtCore/QVariantMap>

#include "Export.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC Properties
{
public:

  void
  put(QString const &name, QVariant const &v);

  template <typename T>
  bool
  get(QString name, T* v) const
  {
    QVariant const &var = _values[name];

    if (var.canConvert<T>())
    {
      *v = _values[name].value<T>();

      return true;
    }

    return false;
  }

  QVariantMap const &
  values() const
  { return _values; }

  QVariantMap &
  values()
  { return _values; }

private:

  QVariantMap _values;
};
}
