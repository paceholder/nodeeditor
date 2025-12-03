#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <QtNodes/qml/QuickGraphModel.hpp>
#include <QtNodes/qml/NodesListModel.hpp>
#include <QtNodes/qml/ConnectionsListModel.hpp>
#include <QtNodes/internal/NodeDelegateModelRegistry.hpp>

#include "QmlNumberSourceDataModel.hpp"
#include "QmlNumberDisplayDataModel.hpp"
#include "AdditionModel.hpp"
#include "MultiplyModel.hpp"
#include "SubtractModel.hpp"
#include "DivideModel.hpp"
#include "FormatNumberModel.hpp"
#include "StringDisplayModel.hpp"
#include "IntegerSourceModel.hpp"
#include "IntegerDisplayModel.hpp"
#include "ToIntegerModel.hpp"
#include "GreaterThanModel.hpp"
#include "BooleanDisplayModel.hpp"

using QtNodes::NodeDelegateModelRegistry;
using QtNodes::QuickGraphModel;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    // Decimal nodes
    ret->registerModel<QmlNumberSourceDataModel>("NumberSource");
    ret->registerModel<QmlNumberDisplayDataModel>("NumberDisplay");
    ret->registerModel<AdditionModel>("Addition");
    ret->registerModel<MultiplyModel>("Multiply");
    ret->registerModel<SubtractModel>("Subtract");
    ret->registerModel<DivideModel>("Divide");
    // String nodes
    ret->registerModel<FormatNumberModel>("FormatNumber");
    ret->registerModel<StringDisplayModel>("StringDisplay");
    // Integer nodes
    ret->registerModel<IntegerSourceModel>("IntegerSource");
    ret->registerModel<IntegerDisplayModel>("IntegerDisplay");
    ret->registerModel<ToIntegerModel>("ToInteger");
    // Boolean nodes
    ret->registerModel<GreaterThanModel>("GreaterThan");
    ret->registerModel<BooleanDisplayModel>("BooleanDisplay");
    return ret;
}

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Fusion");
    QGuiApplication app(argc, argv);

    qmlRegisterType<QuickGraphModel>("QtNodes", 1, 0, "QuickGraphModel");
    qmlRegisterType<QtNodes::NodesListModel>("QtNodes", 1, 0, "NodesListModel");
    qmlRegisterType<QtNodes::ConnectionsListModel>("QtNodes", 1, 0, "ConnectionsListModel");

    qmlRegisterType(QUrl("qrc:/QtNodes/QML/qml/NodeGraph.qml"), "QtNodes", 1, 0, "NodeGraph");
    qmlRegisterType(QUrl("qrc:/QtNodes/QML/qml/Node.qml"), "QtNodes", 1, 0, "Node");
    qmlRegisterType(QUrl("qrc:/QtNodes/QML/qml/Connection.qml"), "QtNodes", 1, 0, "Connection");
    
    auto registry = registerDataModels();
    auto graphModel = new QuickGraphModel();
    graphModel->setRegistry(registry);

    QQmlApplicationEngine engine;
    
    engine.rootContext()->setContextProperty("_graphModel", graphModel);
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
