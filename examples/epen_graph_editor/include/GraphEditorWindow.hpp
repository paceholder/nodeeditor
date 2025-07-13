#ifndef GRAPH_EDITOR_WINDOW
#define GRAPH_EDITOR_WINDOW

#include "QtNodes/GraphicsView"
#include <QPointer>

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

protected:
    // Override to maintain toolbar position
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void createFloatingToolbar();
    void setupNodeCreation();
    
    QPointer<FloatingToolbar> m_toolbar;
    SimpleGraphModel *m_graphModel;
    bool m_toolbarCreated;  // This was missing!
};

#endif // GRAPH_EDITOR_WINDOW