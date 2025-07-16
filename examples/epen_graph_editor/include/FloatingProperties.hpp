#ifndef FLOATINGPROPERTIES_HPP
#define FLOATINGPROPERTIES_HPP

#include <QWidget>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QScrollArea;
class QLabel;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
QT_END_NAMESPACE

class GraphEditorWindow;

class FloatingProperties : public QWidget
{
    Q_OBJECT

public:
    enum DockPosition {
        Floating,
        DockedLeft,
        DockedRight
    };

    explicit FloatingProperties(GraphEditorWindow *parent = nullptr);

    void setDockPosition(DockPosition position);
    DockPosition dockPosition() const { return m_dockPosition; }
    bool isDocked() const { return m_dockPosition != Floating; }
    void updatePosition();

signals:
    void propertyChanged(const QString &name, const QVariant &value);
    void nodeSelected(int nodeId);
    void nodeDeselected();

public slots:
    void updatePropertiesForNode(int nodeId);
    void clearProperties();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void connectSignals();
    DockPosition checkDockingZone(const QPoint &pos);
    void applyDocking(DockPosition position);
    void updateDockedGeometry();
    void clearPropertyWidgets();
    void addPropertyWidget(const QString &label, QWidget *widget);

    GraphEditorWindow *m_graphEditor;
    QVBoxLayout *m_layout;
    QVBoxLayout *m_propertiesLayout;
    QWidget *m_contentWidget;
    QScrollArea *m_scrollArea;
    QPropertyAnimation *m_geometryAnimation;
    
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
    int m_floatHeight;
    
    // Current node
    int m_currentNodeId;
    
    // Property widgets
    QList<QWidget*> m_propertyWidgets;
};

#endif // FLOATINGPROPERTIES_HPP