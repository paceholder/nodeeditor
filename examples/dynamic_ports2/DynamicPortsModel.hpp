#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeDelegateModel>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include <iostream>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class ANodeData : public NodeData
{
public:
    NodeDataType type() const override { return "AData"; }
};

class BNodeData : public NodeData
{
public:
    NodeDataType type() const override { return "BData"; }
};

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
    QJsonObject save() const override;

    void load(QJsonObject const &p) override;

public:
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;

    QWidget *embeddedWidget() override;

    virtual bool resizable() const override { return true; }

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
