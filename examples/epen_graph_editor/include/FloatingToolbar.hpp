#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "DraggableButton.hpp"
#include <QWidget>

class QVBoxLayout;
class QPushButton;
class GraphEditorWindow;
class QPropertyAnimation;
class QScrollArea;

class FloatingToolbar : public QWidget
{
    Q_OBJECT

public:
    enum DockPosition {
        Floating,
        DockedLeft,
        DockedRight
    };

    explicit FloatingToolbar(GraphEditorWindow *parent = nullptr);
    ~FloatingToolbar() = default;

    // Position the toolbar within the parent window
    void updatePosition();

    // Docking functions
    void setDockPosition(DockPosition position);
    DockPosition getDockPosition() const { return m_dockPosition; }
    bool isDocked() const { return m_dockPosition != Floating; }

signals:
    void specificNodeRequested(QString actionName);
    void fillColorChanged(const QColor &color);
    void zoomInRequested();
    void zoomOutRequested();
    void resetViewRequested();

protected:
    // Override for custom painting
    void paintEvent(QPaintEvent *event) override;

    // Mouse events for dragging
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // Override to update position when shown
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void connectSignals();
    QString createSafeButtonText(const QString &icon, const QString &text);
    
    // Docking helpers
    DockPosition checkDockingZone(const QPoint &pos);
    void applyDocking(DockPosition position);
    void updateDockedGeometry();
    
    GraphEditorWindow *m_graphEditor;
    QVBoxLayout *m_layout;
    QWidget *m_contentWidget;
    QScrollArea *m_scrollArea;

    // Dragging support
    bool m_dragging;
    QPoint m_dragStartPosition;
    QRect m_floatingGeometry;  // Remember size and position when floating

    // Docking state
    DockPosition m_dockPosition;
    DockPosition m_previewDockPosition;
    int m_dockMargin;
    int m_dockingDistance; // Distance from edge to trigger docking
    int m_dockedWidth;     // Width when docked
    int _floatHeight;
    
    // Animation
    QPropertyAnimation *m_geometryAnimation;
};

#endif // FLOATING_TOOLBAR_HPP