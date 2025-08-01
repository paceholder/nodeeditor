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
    TestSourceNode()
    {
        _lineEdit = new QLineEdit("Hello World");
        connect(_lineEdit, &QLineEdit::textChanged, this, &TestSourceNode::onTextChanged);
    }

    QString caption() const override { return "Test Source"; }
    QString name() const override { return "TestSourceNode"; }
    static QString Name() { return "TestSourceNode"; }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::Out) ? 1 : 0;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portIndex);
        if (portType == PortType::Out) {
            return TestData{}.type();
        }
        return NodeDataType{};
    }

    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override
    {
        Q_UNUSED(portIndex);
        return std::make_shared<TestData>(_lineEdit->text());
    }

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget* embeddedWidget() override { return _lineEdit; }

    QString getCurrentText() const { return _lineEdit->text(); }
    void setText(const QString& text) { _lineEdit->setText(text); }

private Q_SLOTS:
    void onTextChanged()
    {
        Q_EMIT dataUpdated(0);
    }

private:
    QLineEdit* _lineEdit;
};


// Simple display node that receives and shows test dataR
// And propagates it downstream
class TestDisplayNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    TestDisplayNode()
    {
        _label = new QLabel("No Data");
    }

    QString caption() const override { return "Test Display"; }
    QString name() const override { return "TestDisplayNode"; }
    static QString Name() { return "TestDisplayNode"; }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::In) ? 1 : 1;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return TestData{}.type();
    }

    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override
    {
        Q_UNUSED(portIndex);
        return _receivedData;
    }

    void setInData(std::shared_ptr<NodeData> data,
                   PortIndex const portIndex) override
    {
        Q_UNUSED(portIndex);
        auto d = std::dynamic_pointer_cast<TestData>(data);
        if (d) {
            _receivedData = d;
            _label->setText(d->text());
        } else {
            _receivedData.reset();
            _label->setText("No Data");
        }

        // Propagate downstream
        Q_EMIT dataUpdated(0);
    }

    QWidget* embeddedWidget() override { return _label; }
    QString getText() const {
        if (_receivedData) {
          return _receivedData->text();
        }
        return {};
    }

private:
    QLabel* _label;
    std::shared_ptr<TestData> _receivedData;
};
