#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "DraggableButton.hpp"
#include <QWidget>

class QVBoxLayout;
class QPushButton;
class GraphEditorWindow;

class FloatingToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingToolbar(GraphEditorWindow *parent = nullptr);
    ~FloatingToolbar() = default;

    // Position the toolbar within the parent window
    void updatePosition();

    // Set whether the toolbar is docked to right or left
    void setDockedRight(bool right);
    bool isDockedRight() const { return m_dockedRight; }

signals:
    void rectangleRequested();
    void circleRequested();
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

private:
    void setupUI();
    void connectSignals();
    QString createSafeButtonText(const QString &icon, const QString &text);

    GraphEditorWindow *m_graphEditor;
    QVBoxLayout *m_layout;

    // Dragging support
    bool m_dragging;
    QPoint m_dragStartPosition;

    // Docking position
    bool m_dockedRight;
    int m_margin;
};

#endif // FLOATING_TOOLBAR_HPP