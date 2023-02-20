#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

#include "ImageLoaderModel.hpp"
#include "ImageShowModel.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<ImageShowModel>();

    ret->registerModel<ImageLoaderModel>();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    DataFlowGraphModel dataFlowGraphModel(registry);

    DataFlowGraphicsScene scene(dataFlowGraphModel);

    GraphicsView view(&scene);

    QObject::connect(&scene,
                     &DataFlowGraphicsScene::nodeDoubleClicked,
                     &dataFlowGraphModel,
                     [&dataFlowGraphModel](QtNodes::NodeId nodeId) {
                         QString name = dataFlowGraphModel
                                            .nodeData(nodeId, QtNodes::NodeRole::Caption)
                                            .value<QString>();

                         bool isEmbeded = dataFlowGraphModel
                                              .nodeData(nodeId, QtNodes::NodeRole::WidgetEmbeddable)
                                              .value<bool>();
                         auto w = dataFlowGraphModel.nodeData(nodeId, QtNodes::NodeRole::Widget)
                                      .value<QWidget *>();

                         if (!isEmbeded && w) {
                             w->setWindowTitle(name + "_" + QString::number(nodeId));
                             w->show();
                         }
                     });

    view.setWindowTitle("Data Flow: Resizable Images");
    view.resize(800, 600);
    // Center window.
    view.move(QApplication::primaryScreen()->availableGeometry().center() - view.rect().center());
    view.show();

    return app.exec();
}
