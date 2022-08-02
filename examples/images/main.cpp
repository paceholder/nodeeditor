#include <nodes/DataFlowGraphModel>
#include <nodes/DataFlowGraphicsScene>
#include <nodes/DataModelRegistry>
#include <nodes/GraphicsView>
#include <nodes/NodeData>

#include <QtWidgets/QApplication>
#include <QtGui/QScreen>

#include "ImageShowModel.hpp"
#include "ImageLoaderModel.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataModelRegistry;
using QtNodes::GraphicsView;


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

  std::shared_ptr<DataModelRegistry> registry = registerDataModels();

  DataFlowGraphModel dataFlowGraphModel(registry);

  DataFlowGraphicsScene scene(dataFlowGraphModel);

  GraphicsView view(&scene);

  view.setWindowTitle("Data Flow: Resizable Images");
  view.resize(800, 600);
  // Center window.
  view.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
  view.show();

  return app.exec();
}

