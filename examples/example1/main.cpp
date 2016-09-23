#include <QtWidgets/QApplication>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

#include "models.hpp"


static bool
registerDataModels()
{
  DataModelRegistry::registerModel<NaiveDataModel>();

  /*
   We could have more models registered.
   All of them become items in the context meny of the scene.

  DataModelRegistry::registerModel<AnotherDataModel>();
  DataModelRegistry::registerModel<OneMoreDataModel>();

  */

  return true;
}

static bool registerOK = registerDataModels();


//------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene scene;

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
