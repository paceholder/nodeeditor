#include <nodes/Connection>
#include <nodes/Node>

#include <nodes/FlowScene>
#include <nodes/FlowGraphicsView>

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene::instance();

  FlowScene::instance().setItemIndexMethod(QGraphicsScene::NoIndex);

  FlowScene::instance().createNode();

  FlowGraphicsView view(&FlowScene::instance());

  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView,
                                        "Node-based flow editor"));
  view.resize(800, 600);
  view.show();

  //view.scale(0.5, 0.5);

  return app.exec();
}
