#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include <QWidget>

class QVBoxLayout;
class GraphEditorWindow;

class FloatingToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingToolbar(GraphEditorWindow *parent = nullptr);
    ~FloatingToolbar() = default;

    // Position the toolbar relative to the main window
    void positionRelativeToParent();

signals:
    void rectangleRequested();
    void circleRequested();
    void nodeRequested();
    void fillColorChanged(const QColor &color);
    void zoomInRequested();
    void zoomOutRequested();
    void resetViewRequested();

protected:
    // Override to maintain position relative to parent
    void showEvent(QShowEvent *event) override;

private:
    void setupUI();
    void connectSignals();

    GraphEditorWindow *m_graphEditor;
    QVBoxLayout *m_layout;
};

#endif // FLOATING_TOOLBAR_HPP