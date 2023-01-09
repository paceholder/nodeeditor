#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QAction>
#include <QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>

#include "DataFlowModel.hpp"
#include "DelegateNodeModel.hpp"

using QtNodes::DataFlowGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeRole;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();

    ret->registerModel<SimpleDataModel>();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DataFlowModel graphModel(registerDataModels());

    // Initialize and connect two nodes.
    {
        NodeId id1 = graphModel.addNode(SimpleNodeData().type().id);
        graphModel.setNodeData(id1, NodeRole::Position, QPointF(0, 0));

        NodeId id2 = graphModel.addNode(SimpleNodeData().type().id);
        graphModel.setNodeData(id2, NodeRole::Position, QPointF(300, 300));

        graphModel.addConnection(ConnectionId{id1, 0, id2, 0});
    }

    auto scene = new DataFlowGraphicsScene(graphModel);

    QWidget window;

    QHBoxLayout *l = new QHBoxLayout(&window);

    GraphicsView view(scene);

    l->addWidget(&view);

    QGroupBox *groupBox = new QGroupBox("Options");

    QCheckBox *cb1 = new QCheckBox("Nodes are locked");
    QCheckBox *cb2 = new QCheckBox("Connections detachable");
    cb2->setChecked(true);

    QVBoxLayout *vbl = new QVBoxLayout;
    vbl->addWidget(cb1);
    vbl->addWidget(cb2);
    vbl->addStretch();
    groupBox->setLayout(vbl);

    QObject::connect(cb1, &QCheckBox::stateChanged, [&graphModel](int state) {
        graphModel.setNodesLocked(state == Qt::Checked);
    });

    QObject::connect(cb2, &QCheckBox::stateChanged, [&graphModel](int state) {
        graphModel.setDetachPossible(state == Qt::Checked);
    });

    l->addWidget(groupBox);

    window.setWindowTitle("Locked Nodes and Connections");
    window.resize(800, 600);

    // Center window.
    window.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
    window.showNormal();

    return app.exec();
}
