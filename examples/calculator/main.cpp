#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowGraphicsView>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

#include <nodes/DataModelRegistry>

#include "NumberSourceDataModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "AdditionModel.hpp"
#include "SubtractionModel.hpp"


static bool
registerDataModels()
{
  DataModelRegistry::registerModel
    <NumberSourceDataModel>("Number Source");

  DataModelRegistry::registerModel
    <NumberDisplayDataModel>("Number Display");

  DataModelRegistry::registerModel
    <AdditionModel>("Addition");

  DataModelRegistry::registerModel
    <SubtractionModel>("Subtraction");

  return true;
}

static bool registerOK = registerDataModels();

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  FlowScene scene;

  FlowGraphicsView view(&scene);

  view.setWindowTitle("Dataflow tools: simplest calculator");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
