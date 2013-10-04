#include "Connection.hpp"
#include "FlowItem.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QGraphicsScene scene;
  //  scene.setSceneRect(0, 0, 600, 300);
  scene.setItemIndexMethod(QGraphicsScene::NoIndex);

  Connection* c = new Connection(1, 3);

  scene.addItem(c);

  c = new Connection(1, 3);

  c->setPos(140, 100);
  scene.addItem(c);

  FlowItem* b = new FlowItem();

  b->setPos(0, 200);
  scene.addItem(b);

  QGraphicsView view(&scene);

  view.setRenderHint(QPainter::Antialiasing);
  view.setBackgroundBrush(Qt::darkGray);
  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Qt DBus Controlled Connection"));
  view.resize(800, 600);
  view.show();

  return app.exec();
}
