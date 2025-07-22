#pragma once

#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtTest/QtTest>

#include <catch2/catch.hpp>


namespace Catch {
template<>
struct StringMaker<QPointF>
{
    static std::string convert(QPointF const &p) { return std::string(QTest::toString(p)); }
};

template<>
struct StringMaker<QPoint>
{
    static std::string convert(QPoint const &p) { return std::string(QTest::toString(p)); }
};
} // namespace Catch
