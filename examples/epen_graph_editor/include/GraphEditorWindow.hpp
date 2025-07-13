#ifndef GRAPH_EDITOR_WINDOW
#define GRAPH_EDITOR_WINDOW

#include "QtNodes/GraphicsView"
#include <QPointer>
#include <QMimeData>
#include <QDrag>

using QtNodes::GraphicsView;

class FloatingToolbar;
class SimpleGraphModel;

class GraphEditorWindow : public GraphicsView
{
    Q_OBJECT
public:
    GraphEditorWindow();
    ~GraphEditorWindow();

public slots:
    // Slot for creating a node at a specific position
    void createNodeAtPosition(const QPointF &scenePos);

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
    SimpleGraphModel *m_graphModel;
    bool m_toolbarCreated; // This was missing!
    QString _currentMode;
};

#endif // GRAPH_EDITOR_WINDOW