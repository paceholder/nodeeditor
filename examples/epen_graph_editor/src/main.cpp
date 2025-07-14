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
#include "DataFlowModel.hpp"
#include "data_models/Buffer.hpp"
#include "data_models/Image.hpp"
#include "data_models/Process.hpp"
#include "data_models/VideoInput.hpp"
#include "data_models/VideoOutput.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<Image>();
    ret->registerModel<VideoInput>();
    ret->registerModel<VideoOutput>();
    ret->registerModel<Buffer>();
    ret->registerModel<Process>();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    DataFlowModel dataFlowGraphModel(registry);

    DataFlowGraphicsScene scene(dataFlowGraphModel);

    GraphEditorWindow view(&scene, &dataFlowGraphModel);
    view.showNormal();

    return app.exec();
}
