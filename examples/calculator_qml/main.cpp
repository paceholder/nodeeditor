#include "QmlWrapper.hpp"
#include <QApplication>
#include <QByteArrayView>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QUrl>
#include <QtGlobal>
#include <QtNodes/ConnectionStyle>

using QtNodes::ConnectionStyle;

static void setStyle()
{
    ConnectionStyle::setConnectionStyle(
        R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND", "software");
    QApplication app(argc, argv);

    setStyle();

    qmlRegisterType<WidgetView>("bk", 0, 0, "WidgetView");

    QQmlApplicationEngine engine;
    const QUrl url("qrc:/hello/main.qml");
    engine.load(url);

    return app.exec();
}
