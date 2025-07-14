#include "GraphEditorMainWindow.hpp"
#include "SimpleGraphModel.hpp"
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtWidgets/QApplication>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

#include "data_models/ImageShowModel.hpp"
#include "data_models/VideoOutput.hpp"
#include "data_models/VideoInput.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<ImageShowModel>();
    ret->registerModel<VideoInput>();
    ret->registerModel<VideoOutput>();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    DataFlowGraphModel dataFlowGraphModel(registry);

    DataFlowGraphicsScene scene(dataFlowGraphModel);

    GraphEditorWindow view(&scene, &dataFlowGraphModel);
    view.showNormal();

    return app.exec();
}
