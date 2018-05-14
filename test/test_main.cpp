#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <QApplication>
#include <QtTest>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  app.setAttribute(Qt::AA_Use96Dpi, true);

  return Catch::Session().run(argc, argv);
}
