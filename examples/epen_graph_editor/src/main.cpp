#include "GraphEditorWindow.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GraphEditorWindow view;
    view.showNormal();

    return app.exec();
}
