#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/StyleCollection>

#include <QAction>
#include <QScreen>
#include <QtWidgets/QApplication>

#include "CustomConnectionPainter.hpp"
#include "CustomNodePainter.hpp"
#include "SimpleGraphModel.hpp"

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeRole;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SimpleGraphModel graphModel;

    // Initialize and connect some nodes
    {
        NodeId id1 = graphModel.addNode();
        graphModel.setNodeData(id1, NodeRole::Position, QPointF(0, 0));

        NodeId id2 = graphModel.addNode();
        graphModel.setNodeData(id2, NodeRole::Position, QPointF(300, 50));

        NodeId id3 = graphModel.addNode();
        graphModel.setNodeData(id3, NodeRole::Position, QPointF(300, 200));

        NodeId id4 = graphModel.addNode();
        graphModel.setNodeData(id4, NodeRole::Position, QPointF(600, 100));

        // Create some connections
        graphModel.addConnection(ConnectionId{id1, 0, id2, 0});
        graphModel.addConnection(ConnectionId{id1, 1, id3, 0});
        graphModel.addConnection(ConnectionId{id2, 0, id4, 0});
        graphModel.addConnection(ConnectionId{id3, 0, id4, 1});
    }

    auto scene = new BasicGraphicsScene(graphModel);

    // Set custom painters
    scene->setNodePainter(std::make_unique<CustomNodePainter>());
    scene->setConnectionPainter(std::make_unique<CustomConnectionPainter>());

    GraphicsView view(scene);

    // Setup context menu for creating new nodes
    view.setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction createNodeAction(QStringLiteral("Create Node"), &view);
    QObject::connect(&createNodeAction, &QAction::triggered, [&]() {
        QPointF posView = view.mapToScene(view.mapFromGlobal(QCursor::pos()));

        NodeId const newId = graphModel.addNode();
        graphModel.setNodeData(newId, NodeRole::Position, posView);
    });
    view.insertAction(view.actions().front(), &createNodeAction);

    view.setWindowTitle("Custom Painter Example");
    view.resize(800, 600);

    // Center window
    view.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
    view.showNormal();

    return app.exec();
}
