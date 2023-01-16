#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeDelegateModel>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include <iostream>

using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DynamicPortsModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    virtual ~DynamicPortsModel();

public:
    void init() override;

    QString caption() const override { return "Node"; }

    static QString Name() { return QString("DynamicPortsModel"); }

    QString name() const override { return DynamicPortsModel::Name(); }

public:
    void setInData(QVariant const, PortIndex const) override {}

    QWidget *embeddedWidget() override;

    virtual bool resizable() const override { return true; }

    virtual bool isDynamicPorts() const { return true; }

private:
    QWidget *_widget = nullptr;
    QGridLayout *_layout = nullptr;

    QComboBox *_type = nullptr;
    QSpinBox *_index = nullptr;
    QLineEdit *_name = nullptr;
    QComboBox *_policy = nullptr;
    QComboBox *_dataType = nullptr;
    QPushButton *_insertPort = nullptr;
    QPushButton *_removePort = nullptr;
};
