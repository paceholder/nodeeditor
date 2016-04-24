#include <QtCore>
#include <QtGui>
#include <QtWidgets>

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QGraphicsScene scene;
  QGraphicsView  view(&scene);

  QGraphicsWidget* parentWidget = new QGraphicsWidget();
  parentWidget->setMinimumSize(QSizeF(100, 30));
  parentWidget->setFlags(QGraphicsItem::ItemIsMovable);
  parentWidget->setAutoFillBackground(true);
  scene.addItem(parentWidget);

  QGraphicsProxyWidget *proxy =
    scene.addWidget(new QPushButton("MOVE IT"));
  proxy->setParentItem(parentWidget);

  view.setFixedSize(QSize(600, 400));
  view.show();
  return app.exec();
}
