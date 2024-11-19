#pragma once

#include <QEvent>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QSGNode>
#include <QWidget>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;

class QuickPaintedView : public QQuickPaintedItem
{
    Q_OBJECT
public:
    QuickPaintedView(QQuickItem *parent = nullptr);

protected:
    QSGNode *updatePaintNode(QSGNode *old, UpdatePaintNodeData *d) override;
};

class WidgetView : public QuickPaintedView
{
    Q_OBJECT

public:
    explicit WidgetView(QQuickItem *parent = nullptr);

protected:
    void componentComplete() override;

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
