#pragma once

#include <functional>

#include <QtCore/QUuid>
#include <QtCore/QVariant>

namespace std
{
template<>
struct hash<QUuid>
{
  inline
  std::size_t
  operator()(QUuid const& uid) const
  {
    return qHash(uid);
  }
};
}

