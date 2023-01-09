#include <QAction>
#include <QScreen>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QRadioButton>

#include "SimpleGraphModel.hpp"

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeRole;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SimpleGraphModel graphModel;

    // Initialize and connect two nodes.
    {
        NodeId id1 = graphModel.addNode();
        graphModel.setNodeData(id1, NodeRole::Position, QPointF(0, 0));

        NodeId id2 = graphModel.addNode();
        graphModel.setNodeData(id2, NodeRole::Position, QPointF(300, 300));

        graphModel.addConnection(ConnectionId{id1, 0, id2, 0});
    }

    auto scene = new BasicGraphicsScene(graphModel);

    scene->setOrientation(Qt::Vertical);

    QWidget window;

    QHBoxLayout *l = new QHBoxLayout(&window);

    GraphicsView view(scene);

    l->addWidget(&view);

    QGroupBox *groupBox = new QGroupBox("Orientation");

    QRadioButton *radio1 = new QRadioButton("Vertical");
    QRadioButton *radio2 = new QRadioButton("Horizontal");

    QVBoxLayout *vbl = new QVBoxLayout;
    vbl->addWidget(radio1);
    vbl->addWidget(radio2);
    vbl->addStretch();
    groupBox->setLayout(vbl);

    QObject::connect(radio1, &QRadioButton::clicked, [&scene]() {
        scene->setOrientation(Qt::Vertical);
    });

    QObject::connect(radio2, &QRadioButton::clicked, [&scene]() {
        scene->setOrientation(Qt::Horizontal);
    });

    radio1->setChecked(true);

    l->addWidget(groupBox);

    // Setup context menu for creating new nodes.
    view.setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction createNodeAction(QStringLiteral("Create Node"), &view);
    QObject::connect(&createNodeAction, &QAction::triggered, [&]() {
        // Mouse position in scene coordinates.
        QPointF posView = view.mapToScene(view.mapFromGlobal(QCursor::pos()));

        NodeId const newId = graphModel.addNode();
        graphModel.setNodeData(newId, NodeRole::Position, posView);
    });
    view.insertAction(view.actions().front(), &createNodeAction);

    window.setWindowTitle("Graph Orientation Demo");
    window.resize(800, 600);

    // Center window.
    window.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
    window.showNormal();

    return app.exec();
}
