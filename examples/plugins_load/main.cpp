#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QObject>
#include <QVBoxLayout>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/PluginInterface>
#include <QtNodes/PluginsManager>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::PluginInterface;
using QtNodes::PluginsManager;

void loadPluginsFromFolder()
{
    PluginsManager *pluginsManager = PluginsManager::instance();
    std::shared_ptr<NodeDelegateModelRegistry> registry = pluginsManager->registry();

    // load plugins
    pluginsManager->loadPlugins(QDir::cleanPath(QCoreApplication::applicationDirPath()
                                                + QDir::separator() + "plugins"),
                                QStringList() << "*.node"
                                              << "*.data");

    for (auto l : pluginsManager->loaders()) {
        PluginInterface *plugin = qobject_cast<PluginInterface *>(l.second->instance());
        if (!plugin)
            continue;

        plugin->registerDataModels(registry);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget mainWidget;

    // Load plugins and register models
    loadPluginsFromFolder();

    QVBoxLayout *l = new QVBoxLayout(&mainWidget);

    DataFlowGraphModel dataFlowGraphModel(PluginsManager::instance()->registry());

    auto scene = new DataFlowGraphicsScene(dataFlowGraphModel, &mainWidget);

    auto view = new GraphicsView(scene);
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(scene, &DataFlowGraphicsScene::sceneLoaded, view, &GraphicsView::centerScene);

    mainWidget.setWindowTitle("Data Flow: Plugins Load");
    mainWidget.resize(800, 600);
    mainWidget.show();

    return app.exec();
}
