#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/GraphicsView>
#include <QtNodes/StyleCollection>

#include <QAction>
#include <QFileDialog>
#include <QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

#include "DynamicPortsModel.hpp"

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionStyle;
using QtNodes::GraphicsView;
using QtNodes::NodeRole;
using QtNodes::StyleCollection;

void initializeModel(DynamicPortsModel &graphModel)
{
    NodeId id1 = graphModel.addNode();
    graphModel.setNodeData(id1, NodeRole::Position, QPointF(0, 0));
    graphModel.setNodeData(id1, NodeRole::InPortCount, 1);
    graphModel.setNodeData(id1, NodeRole::OutPortCount, 1);

    NodeId id2 = graphModel.addNode();
    graphModel.setNodeData(id2, NodeRole::Position, QPointF(300, 300));

    graphModel.setNodeData(id2, NodeRole::InPortCount, 1);
    graphModel.setNodeData(id2, NodeRole::OutPortCount, 1);

    graphModel.addConnection(ConnectionId{id1, 0, id2, 0});
}

QMenuBar *createSaveRestoreMenu(DynamicPortsModel &graphModel,
                                BasicGraphicsScene *scene,
                                GraphicsView &view)
{
    auto menuBar = new QMenuBar();
    QMenu *menu = menuBar->addMenu("File");
    auto saveAction = menu->addAction("Save Scene");
    auto loadAction = menu->addAction("Load Scene");

    QObject::connect(saveAction, &QAction::triggered, scene, [&graphModel] {
        QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                        "Open Flow Scene",
                                                        QDir::homePath(),
                                                        "Flow Scene Files (*.flow)");

        if (!fileName.isEmpty()) {
            if (!fileName.endsWith("flow", Qt::CaseInsensitive))
                fileName += ".flow";

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(QJsonDocument(graphModel.save()).toJson());
            }
        }
    });

    QObject::connect(loadAction, &QAction::triggered, scene, [&graphModel, &view, scene] {
        QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                        "Open Flow Scene",
                                                        QDir::homePath(),
                                                        "Flow Scene Files (*.flow)");
        if (!QFileInfo::exists(fileName))
            return;

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))
            return;

        scene->clearScene();

        QByteArray const wholeFile = file.readAll();

        graphModel.load(QJsonDocument::fromJson(wholeFile).object());

        view.centerScene();
    });

    return menuBar;
}

QAction *createNodeAction(DynamicPortsModel &graphModel, GraphicsView &view)
{
    auto action = new QAction(QStringLiteral("Create Node"), &view);
    QObject::connect(action, &QAction::triggered, [&]() {
        // Mouse position in scene coordinates.
        QPointF posView = view.mapToScene(view.mapFromGlobal(QCursor::pos()));

        NodeId const newId = graphModel.addNode();
        graphModel.setNodeData(newId, NodeRole::Position, posView);
    });

    return action;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DynamicPortsModel graphModel;

    // Initialize and connect two nodes.
    initializeModel(graphModel);

    // Main app window holding menu and a scene view.
    QWidget window;
    window.setWindowTitle("Dynamic Nodes Example");
    window.resize(800, 600);

    auto scene = new BasicGraphicsScene(graphModel);

    qWarning() << "MODEF FROM SCENE " << &(scene->graphModel());

    GraphicsView view(scene);
    // Setup context menu for creating new nodes.
    view.setContextMenuPolicy(Qt::ActionsContextMenu);
    view.insertAction(view.actions().front(), createNodeAction(graphModel, view));

    // Pack all elements into layout.
    QVBoxLayout *l = new QVBoxLayout(&window);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(createSaveRestoreMenu(graphModel, scene, view));
    l->addWidget(&view);

    // Center window
    window.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
    window.showNormal();

    return app.exec();
}
