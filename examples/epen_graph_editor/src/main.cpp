#include "GraphEditorMainWindow.hpp"
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtWidgets/QApplication>

#include "DataFlowModel.hpp"
#include "data_models/FixedBuffer.hpp"
#include "data_models/Process.hpp"
#include "data_models/SliderBuffer_unsignedInt.hpp"
#include "data_models/VideoInput.hpp"
#include "data_models/VideoOutput.hpp"
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<VideoInput>();
    ret->registerModel<VideoOutput>();
    ret->registerModel<FixedBuffer>();
    ret->registerModel<Process>();

    ret->registerModel<SliderBuffer_unsignedInt>();
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
