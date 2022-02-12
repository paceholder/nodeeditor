#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>

#include <QtWidgets/QApplication>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"

using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::GraphicsView;

static std::shared_ptr<NodeDelegateModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<NodeDelegateModelRegistry>();

  ret->registerModel<TextSourceDataModel>();
  ret->registerModel<TextDisplayDataModel>();

  return ret;
}


int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();
  DataFlowGraphModel dataFlowGraphModel(registry);

  DataFlowGraphicsScene scene(dataFlowGraphModel);

  GraphicsView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}

