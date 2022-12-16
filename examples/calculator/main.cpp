#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

#include <QtGui/QScreen>

#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "SubtractionModel.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<NumberSourceDataModel>("Sources");

    ret->registerModel<NumberDisplayDataModel>("Displays");

    ret->registerModel<AdditionModel>("Operators");

    ret->registerModel<SubtractionModel>("Operators");

    ret->registerModel<MultiplicationModel>("Operators");

    ret->registerModel<DivisionModel>("Operators");

    return ret;
}

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
    QApplication app(argc, argv);

    setStyle();

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    QWidget mainWidget;

    auto menuBar = new QMenuBar();
    QMenu *menu = menuBar->addMenu("File");
    auto saveAction = menu->addAction("Save Scene");
    auto loadAction = menu->addAction("Load Scene");

    QVBoxLayout *l = new QVBoxLayout(&mainWidget);

    DataFlowGraphModel dataFlowGraphModel(registry);

    l->addWidget(menuBar);
    auto scene = new DataFlowGraphicsScene(dataFlowGraphModel, &mainWidget);

    auto view = new GraphicsView(scene);
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(saveAction, &QAction::triggered, scene, &DataFlowGraphicsScene::save);

    QObject::connect(loadAction, &QAction::triggered, scene, &DataFlowGraphicsScene::load);

    QObject::connect(scene, &DataFlowGraphicsScene::sceneLoaded, view, &GraphicsView::centerScene);

    mainWidget.setWindowTitle("Data Flow: simplest calculator");
    mainWidget.resize(800, 600);
    // Center window.
    mainWidget.move(QApplication::primaryScreen()->availableGeometry().center()
                    - mainWidget.rect().center());
    mainWidget.showNormal();

    return app.exec();
}
