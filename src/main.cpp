#include "Connection.hpp"
#include "FlowItem.hpp"

#include "FlowScene.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  FlowScene::instance();
  FlowScene::instance()->setItemIndexMethod(QGraphicsScene::NoIndex);

  FlowScene::instance()->createFlowItem();

  QGraphicsView view(FlowScene::instance());

  view.setRenderHint(QPainter::Antialiasing);
  view.setBackgroundBrush(QColor(Qt::gray).darker());
  view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Node-based flow editor"));
  view.resize(800, 600);
  view.show();

  return app.exec();
}
