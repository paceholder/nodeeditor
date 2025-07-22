#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DataFlowModel;

class PortAddRemoveWidget : public QWidget
{
    Q_OBJECT

public:
    PortAddRemoveWidget(NodeId nodeId, DataFlowModel &model, QWidget *parent = nullptr);
    ~PortAddRemoveWidget();


private slots:
    void addLeftPort();
    void removeLeftPort();
    void addRightPortI();
    void addRightPortB();
    void removeRightPort();

private:
    void addRightPort(bool isRight);
    std::pair<PortType, PortIndex> findWhichPortWasClicked(QObject *sender, int const buttonIndex);

private:
    NodeId _nodeId;
    DataFlowModel &_model;
    QVBoxLayout *_left;
    QVBoxLayout *_right;
    int _leftPorts = 0;
    int _rightPorts = 0;
};