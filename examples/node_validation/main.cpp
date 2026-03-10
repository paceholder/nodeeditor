#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>

#include "TextDisplayModel.hpp"
#include "TextSourceModel.hpp"
#include "ValidatedModel.hpp"

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();

    ret->registerModel<TextSourceModel>("Sources");
    ret->registerModel<ValidatedModel>("Processors");
    ret->registerModel<TextDisplayModel>("Displays");

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();

    QWidget mainWidget;
    QVBoxLayout *layout = new QVBoxLayout(&mainWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    DataFlowGraphModel dataFlowGraphModel(registry);

    auto scene = new DataFlowGraphicsScene(dataFlowGraphModel, &mainWidget);
    auto view = new GraphicsView(scene);
    layout->addWidget(view);

    mainWidget.setWindowTitle("Node Validation Example");
    mainWidget.resize(800, 600);
    mainWidget.move(QApplication::primaryScreen()->availableGeometry().center()
                    - mainWidget.rect().center());
    mainWidget.showNormal();

    return app.exec();
}
