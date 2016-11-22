#include <QtWidgets/QApplication>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

#include "models.hpp"


static bool
registerDataModels()
{
  DataModelRegistry::registerModel(std::unique_ptr<NaiveDataModel>(new NaiveDataModel));

  /*
   We could have more models registered.
   All of them become items in the context meny of the scene.

  DataModelRegistry::registerModel(std::unique_ptr<AnotherDataModel>(new AnotherDataModel));
  DataModelRegistry::registerModel(std::unique_ptr<OneMoreDataModel>(new OneMoreDataModel));

  */

  return true;
}


//------------------------------------------------------------------------------

int
main(int argc, char* argv[])
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
