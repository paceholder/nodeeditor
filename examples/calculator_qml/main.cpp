#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/QmlWrapper>

#include <QQmlApplicationEngine>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QPushButton>

#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "MyNodeEditor.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "StringDataModel.hpp"
#include "SubtractionModel.hpp"

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

    qmlRegisterType<MyNodeEditor>("bk", 0, 0, "NodeEditor");

    QQmlApplicationEngine engine;
    const QUrl url("qrc:/hello/main.qml");
    engine.load(url);

    return app.exec();
}
