#include "GraphEditorWindow.hpp"
#include "SimpleGraphModel.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create the graph model
    SimpleGraphModel *graphModel = new SimpleGraphModel();

    // Create and set the scene
    auto scene = new BasicGraphicsScene(*graphModel);

    GraphEditorWindow view(scene);
    view.showNormal();

    return app.exec();
}
