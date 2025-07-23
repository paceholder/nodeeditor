#pragma once

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>

#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

// Simple test data type for data flow testing
class TestData : public NodeData
{
public:
    TestData() {}
    TestData(QString text) : _text(text) {}

    NodeDataType type() const override
    {
        return NodeDataType{"TestData", "Test Data"};
    }

    QString text() const { return _text; }
    void setText(const QString& text) { _text = text; }

private:
    QString _text;
};

// Simple source node that outputs test data
class TestSourceNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    TestSourceNode();

    QString caption() const override { return "Test Source"; }
    QString name() const override { return "TestSourceNode"; }
    static QString Name() { return "TestSourceNode"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override;
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget* embeddedWidget() override { return _lineEdit; }

    QString getCurrentText() const { return _lineEdit->text(); }
    void setText(const QString& text) { _lineEdit->setText(text); }

private Q_SLOTS:
    void onTextChanged();

private:
    QLineEdit* _lineEdit;
};

// Simple display node that receives and shows test data
class TestDisplayNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    TestDisplayNode();

    QString caption() const override { return "Test Display"; }
    QString name() const override { return "TestDisplayNode"; }
    static QString Name() { return "TestDisplayNode"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex const portIndex) override;

    QWidget* embeddedWidget() override { return _label; }

    QString getReceivedData() const { return _receivedData; }

private:
    QLabel* _label;
    QString _receivedData;
};
