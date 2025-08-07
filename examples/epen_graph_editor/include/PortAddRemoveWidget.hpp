#pragma once

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>
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

    void rootSelected();

    void addLeftPortView(bool isImage);
private slots:
    void addRightPortI();
    void addRightPortB();
    void addLeftPortI();
    void addLeftPortB();
    void removeLeftPort();
    void removeRightPort();
    void onLeftRadioButtonToggled(bool checked);
    void onRightRadioButtonToggled(bool checked);
    void leftMinusClicked();
    void rightMinusClicked();

private:
    void addRightPort(bool);
    void addLeftPort(bool);
    int findWhichRadioWasClicked(QVBoxLayout *, QObject *);

private:
    NodeId _nodeId;
    DataFlowModel &_model;
    QVBoxLayout *_left;
    QVBoxLayout *_right;
    int _leftPorts = 0;
    int _rightPorts = 0;

    // Radio button group to ensure only one is selected across both sides
    QButtonGroup *_radioGroup;

    QPushButton *_letMinusButton;
    QPushButton *_rightMinusButton;

    int _selectedLeftPortIndex = -1;
    int _selectedRightPortIndex = -1;
};