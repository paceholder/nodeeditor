#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/DataModelRegistry>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>

#include <QtWidgets/QApplication>

#include <QtNodes/DataModelRegistry>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"

using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataModelRegistry;
using QtNodes::GraphicsView;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<TextSourceDataModel>();
  ret->registerModel<TextDisplayDataModel>();

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

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
