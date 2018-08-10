#include <nodes/NodeData>
#include <nodes/DataFlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include <nodes/DataModelRegistry>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::FlowView;
using QtNodes::DataFlowScene;

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

  DataFlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
