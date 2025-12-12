#pragma once

#include <QQuickPaintedItem>
#include <QVariant>
#include <QWidget>
#include <QPushButton>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/DataFlowGraphicsScene>

using QtNodes::GraphicsView;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;

namespace QtNodes {

class NODE_EDITOR_PUBLIC QuickPaintedView : public QQuickPaintedItem
{
    Q_OBJECT
public:
    QuickPaintedView(QQuickItem *parent = nullptr);

protected:
    QSGNode *updatePaintNode(QSGNode *old, UpdatePaintNodeData *d) override;
};

class NODE_EDITOR_PUBLIC QmlWrapper : public QuickPaintedView
{
    Q_OBJECT

public:
    explicit QmlWrapper(QQuickItem *parent = nullptr);

protected:
    void componentComplete() override;
    virtual std::shared_ptr<NodeDelegateModelRegistry> registerDataModels() const = 0;

private:
    void paint(QPainter *painter) override;
    bool event(QEvent *event) override;
    bool handleHoverEvent(QHoverEvent *event);
    bool handleMouseEvent(QMouseEvent *event);

    QWidget *qWidget() const;
    void updateSizeConstraints();

    void setWidget();

    GraphicsView *m_widget = nullptr;
    DataFlowGraphicsScene *m_widget_scene = nullptr;
    DataFlowGraphModel *m_model = nullptr;
};

} // namespace QtNodes
