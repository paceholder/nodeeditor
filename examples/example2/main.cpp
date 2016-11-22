#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include <nodes/DataModelRegistry>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"


static DataModelRegistry
registerDataModels()
{
  DataModelRegistry ret;
  ret.registerModel(std::unique_ptr<TextSourceDataModel>(new TextSourceDataModel));

  ret.registerModel(std::unique_ptr<TextDisplayDataModel>(new TextDisplayDataModel));

  return ret;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  FlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
