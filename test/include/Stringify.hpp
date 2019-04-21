#pragma once

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QString>

#include <catch2/catch.hpp>

#include <QtTest>

namespace Catch
{
template <>
struct StringMaker<QPointF>
{
  static std::string
  convert(QPointF const& p)
  {
    return std::string(QTest::toString(p));
  }
};

template <>
struct StringMaker<QPoint>
{
  static std::string
  convert(QPoint const& p)
  {
    return std::string(QTest::toString(p));
  }
};

template <>
struct StringMaker<QString>
{
  static std::string
  convert(QString const& str)
  {
    return str.toStdString();
  }
};
}
