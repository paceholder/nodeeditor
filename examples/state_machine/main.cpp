#include "state_node_model.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>

#include <QApplication>
#include <QMenuBar>
#include <QBoxLayout>

using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::DataModelRegistry;
using QtNodes::ConnectionStyle;

std::shared_ptr<DataModelRegistry> registerDataModels()
{
    std::shared_ptr<DataModelRegistry> registry(new DataModelRegistry());

    registry->registerModel<StateNodeModel>();

    return registry;
}


static
void
setStyle()
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

    QWidget mainWidget;

    auto menuBar = new QMenuBar();
    auto saveAction = menuBar->addAction("Save..");
    auto loadAction = menuBar->addAction("Load..");

    QVBoxLayout *l = new QVBoxLayout(&mainWidget);

    l->addWidget(menuBar);
    auto scene = new FlowScene(registerDataModels(), &mainWidget);
    l->addWidget(new FlowView(scene));
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(saveAction, &QAction::triggered,
        scene, &FlowScene::save);

    QObject::connect(loadAction, &QAction::triggered,
        scene, &FlowScene::load);

    mainWidget.setWindowTitle("Simplest state editor");
    mainWidget.resize(800, 600);
    mainWidget.showNormal();

    return app.exec();
}