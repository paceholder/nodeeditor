#include <QtWidgets/QApplication>

#include <nodes/ConnectionStyle>
#include <nodes/DataFlowGraphModel>
#include <nodes/DataFlowGraphicsScene>
#include <nodes/DataModelRegistry>
#include <nodes/GraphicsView>
#include <nodes/NodeData>

#include "models.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::ConnectionStyle;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<NaiveDataModel>();

  /*
     We could have more models registered.
     All of them become items in the context meny of the scene.

     ret->registerModel<AnotherDataModel>();
     ret->registerModel<OneMoreDataModel>();

   */

  return ret;
}


static
void
setStyle()
{
  ConnectionStyle::setConnectionStyle(
    R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true
    }
  }
  )");
}


//------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  setStyle();

  std::shared_ptr<DataModelRegistry> registry = registerDataModels();
  DataFlowGraphModel dataFlowGraphModel(registry);

  DataFlowGraphicsScene scene(dataFlowGraphModel);

  GraphicsView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
