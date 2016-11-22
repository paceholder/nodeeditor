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

static bool
registerDataModels()
{
  DataModelRegistry::registerModel(std::unique_ptr<NumberSourceDataModel>(new NumberSourceDataModel));

  DataModelRegistry::registerModel(std::unique_ptr<NumberDisplayDataModel>(new NumberDisplayDataModel));

  DataModelRegistry::registerModel(std::unique_ptr<AdditionModel>(new AdditionModel));

  DataModelRegistry::registerModel(std::unique_ptr<SubtractionModel>(new SubtractionModel));

  DataModelRegistry::registerModel(std::unique_ptr<MultiplicationModel>(new MultiplicationModel));

  DataModelRegistry::registerModel(std::unique_ptr<DivisionModel>(new DivisionModel));

  return true;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  bool success = registerDataModels();
  Q_ASSERT(success);
  
  QWidget mainWidget;

  auto menuBar    = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);

  l->addWidget(menuBar);
  auto scene = new FlowScene;
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
