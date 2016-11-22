#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>

#include <nodes/DataModelRegistry>

#include "NumberSourceDataModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "AdditionModel.hpp"
#include "SubtractionModel.hpp"
#include "MultiplicationModel.hpp"
#include "DivisionModel.hpp"

static DataModelRegistry
registerDataModels()
{
  DataModelRegistry ret;
  ret.registerModel(std::make_unique<NumberSourceDataModel>());

  ret.registerModel(std::make_unique<NumberDisplayDataModel>());

  ret.registerModel(std::make_unique<AdditionModel>());

  ret.registerModel(std::make_unique<SubtractionModel>());

  ret.registerModel(std::make_unique<MultiplicationModel>());

  ret.registerModel(std::make_unique<DivisionModel>());

  return ret;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  QWidget mainWidget;

  auto menuBar    = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);

  l->addWidget(menuBar);
  auto scene = new FlowScene(registerDataModels());
  l->addWidget(new FlowView(scene));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   scene, &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   scene, &FlowScene::load);

  mainWidget.setWindowTitle("Dataflow tools: simplest calculator");
  mainWidget.resize(800, 600);
  mainWidget.showNormal();

  return app.exec();
}
