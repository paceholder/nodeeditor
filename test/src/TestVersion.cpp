#include <QtNodes/Version>

#include <catch2/catch.hpp>

#include <string>

TEST_CASE("The version string is built from the version numbers", "[core][version]")
{
    std::string const expected = std::to_string(QTNODES_VERSION_MAJOR) + "."
                                 + std::to_string(QTNODES_VERSION_MINOR) + "."
                                 + std::to_string(QTNODES_VERSION_PATCH);

    CHECK(std::string(QTNODES_VERSION_STR) == expected);
}

TEST_CASE("Newer versions compare greater with QTNODES_VERSION_CHECK", "[core][version]")
{
    CHECK(QTNODES_VERSION_CHECK(3, 2, 1) < QTNODES_VERSION_CHECK(3, 2, 2));
    CHECK(QTNODES_VERSION_CHECK(3, 2, 9) < QTNODES_VERSION_CHECK(3, 3, 0));
    CHECK(QTNODES_VERSION_CHECK(3, 9, 9) < QTNODES_VERSION_CHECK(4, 0, 0));
    CHECK(QTNODES_VERSION >= QTNODES_VERSION_CHECK(QTNODES_VERSION_MAJOR, 0, 0));
}
