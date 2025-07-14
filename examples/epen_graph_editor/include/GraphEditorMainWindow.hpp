#ifndef GRAPH_EDITOR_WINDOW
#define GRAPH_EDITOR_WINDOW

#include "QtNodes/GraphicsView"
#include <QDrag>
#include <QMimeData>
#include <QPointer>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include "DataFlowModel.hpp"

using QtNodes::BasicGraphicsScene;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;

class FloatingToolbar;
class SimpleGraphModel;

class GraphEditorWindow : public GraphicsView
{
    Q_OBJECT
public:
    GraphEditorWindow(DataFlowGraphicsScene *scene, DataFlowModel *model);
    ~GraphEditorWindow();

public slots:
    // Slot for creating a node at a specific position
    void createNodeAtPosition(const QPointF &scenePos,const QString nodeType);

    // Slot for creating a node at cursor position
    void createNodeAtCursor();
    void goToMode(QString mode);

protected:
    // Override to maintain toolbar position
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    void createFloatingToolbar();
    void setupNodeCreation();

    QPointer<FloatingToolbar> m_toolbar;
    //SimpleGraphModel *m_graphModel;
    bool m_toolbarCreated; // This was missing!
    QString _currentMode;
    DataFlowModel *_model;
};

#endif // GRAPH_EDITOR_WINDOW