#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include "ImageShowModel.hpp"
#include "ImageLoaderModel.hpp"

static DataModelRegistry
registerDataModels()
{
  DataModelRegistry ret;
  ret.registerModel(std::unique_ptr<ImageShowModel>(new ImageShowModel));

  ret.registerModel(std::unique_ptr<ImageLoaderModel>(new ImageLoaderModel));

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
