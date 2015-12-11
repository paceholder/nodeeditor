#include "Connection.hpp"
#include "Node.hpp"

#include "FlowScene.hpp"
#include "FlowGraphicsView.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene::instance();

  // TODO what is it?
  //FlowScene::instance().setItemIndexMethod(QGraphicsScene::NoIndex);

  FlowScene::instance().createNode();

  FlowGraphicsView view(&FlowScene::instance());

  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView,
                                        "Node-based flow editor"));
  view.resize(800, 600);
  view.show();

  //view.scale(0.5, 0.5);

  return app.exec();
}
