#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include "ImageShowModel.hpp"
#include "ImageLoaderModel.hpp"

static bool
registerDataModels()
{
  DataModelRegistry::registerModel(std::make_unique<ImageShowModel>());

  DataModelRegistry::registerModel(std::make_unique<ImageLoaderModel>());

  return true;
}


int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  bool success = registerDataModels();
  Q_ASSERT(success);
  FlowScene scene;

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
