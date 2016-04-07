#include <iostream>

#include <nodes/Connection>
#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowGraphicsView>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

#include <nodes/DataModelRegistry>

#include "models.hpp"


static bool
registerDataModels()
{
  DataModelRegistry::registerModel<NaiveDataModel>("NaiveDataModel");

  return true;
}


static bool registerOK = registerDataModels();

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  //FlowScene::instance();


  FlowGraphicsView view(&FlowScene::instance());

  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView,
                                        "Node-based flow editor"));
  view.resize(800, 600);
  view.show();

  //view.scale(0.5, 0.5);

  return app.exec();
}
