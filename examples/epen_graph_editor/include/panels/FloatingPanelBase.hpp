#ifndef FLOATING_PANEL_BASE_HPP
#define FLOATING_PANEL_BASE_HPP

#include <QWidget>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QScrollArea;
class QLabel;
QT_END_NAMESPACE

class GraphEditorWindow;

class FloatingPanelBase : public QWidget
{
    Q_OBJECT

public:
    enum DockPosition {
        Floating,
        DockedLeft,
        DockedRight,
        DockedBottom  // Add bottom docking support
    };

    explicit FloatingPanelBase(GraphEditorWindow *parent = nullptr, const QString &title = "Panel");
    virtual ~FloatingPanelBase();

    // Docking API
    void setDockPosition(DockPosition position);
    DockPosition dockPosition() const { return m_dockPosition; }
    bool isDocked() const { return m_dockPosition != Floating; }
    void updatePosition();

    // Panel configuration
    void setDockedWidth(int width) { m_dockedWidth = width; }
    int dockedWidth() const { return m_dockedWidth; }
    void setFloatingWidth(int width) { m_floatingWidth = width; }
    int floatingWidth() const { return m_floatingWidth; }
    void setDockedHeight(int height) { m_dockedHeight = height; }  // For bottom docking
    int dockedHeight() const { return m_dockedHeight; }
    void setDockingDistance(int distance) { m_dockingDistance = distance; }
    
    // Priority system
    void setPanelPriority(int priority) { m_priority = priority; }
    int panelPriority() const { return m_priority; }
    
    // Resize control
    void setResizable(bool resizable) { m_isResizable = resizable; }
    bool isResizable() const { return m_isResizable; }

signals:
    void dockPositionChanged(DockPosition newPosition);

protected:
    // Override these to customize panel behavior
    virtual void setupUI() = 0;
    virtual void connectSignals() = 0;
    virtual QString getPanelStyleSheet() const;
    virtual QString getContentStyleSheet() const;
    virtual void dockChanged(bool isFloat);
    
    // Event handlers
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    // Helper methods for derived classes
    void setupBaseUI(const QString &title);
    GraphEditorWindow *getGraphEditor() const { return m_graphEditor; }
    QVBoxLayout *getContentLayout() const { return m_contentLayout; }
    QWidget *getContentWidget() const { return m_contentWidget; }
    QScrollArea *getScrollArea() const { return m_scrollArea; }

private:
    // Resize handling
    enum ResizeEdge {
        NoEdge,
        TopEdge,
        BottomEdge,
        LeftEdge,
        RightEdge,
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner
    };
    
    ResizeEdge getResizeEdge(const QPoint &pos);
    void updateCursor(const QPoint &pos);
    
    // Docking helpers
    DockPosition checkDockingZone(const QPoint &pos);
    void applyDocking(DockPosition position);
    void updateDockedGeometry();
    QRect calculateDockedGeometry(DockPosition position);
    void updateAllDockedPanels();
    int getBottomPanelHeight() const;

protected:
    GraphEditorWindow *m_graphEditor;
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_contentLayout;
    QWidget *m_contentWidget;
    QScrollArea *m_scrollArea;
    QPropertyAnimation *m_geometryAnimation;
    QLabel *m_titleLabel;
    
    // Dragging state
    bool m_dragging;
    QPoint m_dragStartPosition;
    DockPosition m_dockPosition;
    DockPosition m_previewDockPosition;
    QRect m_floatingGeometry;
    
    // Docking settings
    int m_dockMargin;
    int m_dockingDistance;
    int m_dockedWidth;
    int m_dockedHeight;  // Height when docked to bottom
    int m_floatingWidth;
    int m_floatHeight;
    int m_priority;      // Panel priority (higher = more important)
    
    // Panel title
    QString m_panelTitle;
    
    // Resize control
    bool m_isResizable;
    
    // Resize state
    bool m_resizing;
    ResizeEdge m_resizeEdge;
    QPoint m_resizeStartPos;
    QRect m_resizeStartGeometry;
    int m_resizeStartHeight;
    static const int RESIZE_MARGIN = 10;  // Increased from 8 to 10 for easier grabbing
};

#endif // FLOATING_PANEL_BASE_HPP