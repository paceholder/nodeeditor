#include <QtWidgets/QApplication>

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

#include "RiverListModel.h"
#include "RiverModel.h"


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;


static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<RiverListModel>();
  ret->registerModel<RiverModel>();

  return ret;
}


int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Dynamic ports example");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
