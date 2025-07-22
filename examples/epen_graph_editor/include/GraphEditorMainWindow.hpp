#ifndef GRAPH_EDITOR_WINDOW
#define GRAPH_EDITOR_WINDOW

#include "DataFlowModel.hpp"
#include "QtNodes/GraphicsView"
#include "data_models/OperationDataModel.hpp"
#include <QDrag>
#include <QMimeData>
#include <QPointer>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>

using QtNodes::BasicGraphicsScene;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::InvalidNodeId;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeId;

class FloatingToolbar;
class FloatingProperties;
class SimpleGraphModel;

class GraphEditorWindow : public GraphicsView
{
    Q_OBJECT
public:
    GraphEditorWindow(DataFlowGraphicsScene *scene, DataFlowModel *model);
    ~GraphEditorWindow();

public slots:
    // Slot for creating a node at a specific position
    void createNodeAtPosition(const QPointF &scenePos, const QString nodeType);

    // Slot for creating a node at cursor position
    void createNodeAtCursor();
    void goToMode(QString mode);

    // Slots for node selection and property changes
    void onNodeSelected(NodeId nodeId);
    void onNodeDeselected();
    void onPropertyChanged(const QString &name, const QVariant &value);

protected:
    // Override to maintain toolbar position
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &r) override
    {
        GraphicsView::drawBackground(painter, r);

        if (_allowedDropAreaLeft != -1) {
            float left = _allowedDropAreaLeft;
            if (left == -2)
                left = r.left();
            if (left <= r.right()) {
                if (left < r.left())
                    left = r.left();
                // Set anti-aliasing (optional for smoother edges)
                painter->setRenderHint(QPainter::Antialiasing);

                QColor semiTransparentBlue(0, 0, 255, 50);

                // Set brush with the color
                painter->setBrush(QBrush(semiTransparentBlue));

                // Set pen to no border (optional)
                painter->setPen(Qt::NoPen);

                // Draw the rectangle
                painter->drawRect(QRectF(left, r.top(), r.right() - left, r.height()));
            }
        }
    }

private:
    void createFloatingToolbar();
    void createFloatingProperties();
    void setupNodeCreation();

    QPointer<FloatingToolbar> m_toolbar;
    QPointer<FloatingProperties> m_properties;

    bool m_toolbarCreated;
    bool m_propertiesCreated;
    QString _currentMode;
    DataFlowModel *_model;
    NodeId m_currentSelectedNodeId;

    float _allowedDropAreaLeft = -1;
};

#endif // GRAPH_EDITOR_WINDOW