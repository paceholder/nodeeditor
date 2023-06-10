#include "QtNodes/NodeGraphicsView.hpp"

nitro::NodeGraphicsView::NodeGraphicsView(QtNodes::BasicGraphicsScene *scene,
                                          QtNodes::DataFlowGraphModel *model, QWidget *parent)
        : QtNodes::GraphicsView(scene, parent),
          dataModel_(model),
          scene_(scene) {
    auto *spawnMenu = new QAction(QStringLiteral("Add node"), this);
    spawnMenu->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
    spawnMenu->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_A));
    connect(spawnMenu, &QAction::triggered, this, &NodeGraphicsView::spawnNodeMenu);
    insertAction(actions().front(), spawnMenu);
    setScaleRange(0.3, 1.5);
}

QtNodes::BasicGraphicsScene *nitro::NodeGraphicsView::getScene() const {
    return scene_;
}

void nitro::NodeGraphicsView::spawnNodeMenu() {
    getNodeMenu()->popup(QCursor::pos());
    setDragMode(QGraphicsView::ScrollHandDrag);
}

QMenu *nitro::NodeGraphicsView::getNodeMenu() {
    if (!nodeMenu_) {
        nodeMenu_ = initNodeMenu();
    }
    return nodeMenu_;
}

QtNodes::DataFlowGraphModel *nitro::NodeGraphicsView::getDataModel() const {
    return dataModel_;
}
