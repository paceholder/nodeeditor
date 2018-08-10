#include <nodes/NodeData>
#include <nodes/DataFlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include "ImageShowModel.hpp"
#include "ImageLoaderModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::DataFlowScene;
using QtNodes::FlowView;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<ImageShowModel>();

  ret->registerModel<ImageLoaderModel>();

  return ret;
}


int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  DataFlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
