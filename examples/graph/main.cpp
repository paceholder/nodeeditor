#include <QtNodes/ConnectionStyle>
#include <QtNodes/GraphicsView>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/StyleCollection>

#include <QtWidgets/QApplication>
#include <QAction>
#include <QScreen>

#include "CustomGraphModel.hpp"


using QtNodes::ConnectionStyle;
using QtNodes::GraphicsView;
using QtNodes::BasicGraphicsScene;
using QtNodes::NodeRole;
using QtNodes::StyleCollection;

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  CustomGraphModel graphModel;

  // Initialize and connect two nodes.
  {
    NodeId id1 = graphModel.addNode();
    graphModel.setNodeData(id1, NodeRole::Position, QPointF(0, 0));

    NodeId id2 = graphModel.addNode();
    graphModel.setNodeData(id2, NodeRole::Position, QPointF(300, 300));

    graphModel.addConnection(ConnectionId{id1, 0, id2, 0});
  }

  auto scene = new BasicGraphicsScene(graphModel);

  GraphicsView view(scene);

  // Setup context menu for creating new nodes.
  view.setContextMenuPolicy(Qt::ActionsContextMenu);
  QAction createNodeAction(QStringLiteral("Create Node"), &view);
  QObject::connect(&createNodeAction, &QAction::triggered,
                   [&]()
                   {
                     // Mouse position in scene coordinates.
                     QPointF posView =
                       view.mapToScene(view.mapFromGlobal(QCursor::pos()));


                     NodeId const newId = graphModel.addNode();
                     graphModel.setNodeData(newId,
                                            NodeRole::Position,
                                            posView);
                   });
  view.insertAction(view.actions().front(), &createNodeAction);


  view.setWindowTitle("Simple Node Graph");
  view.resize(800, 600);

  // Center window.
  view.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
  view.showNormal();

  return app.exec();
}

