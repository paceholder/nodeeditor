#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "DecimalData.hpp"
#include "BooleanData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class GreaterThanModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(QString resultText READ resultText NOTIFY resultChanged)

public:
    QString caption() const override { return QStringLiteral("A > B"); }
    QString name() const override { return QStringLiteral("GreaterThan"); }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::In) ? 2 : 1;
    }

    NodeDataType dataType(PortType portType, PortIndex) const override
    {
        if (portType == PortType::In) {
            return DecimalData{}.type();
        }
        return BooleanData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override
    {
        auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
        if (portIndex == 0) {
            _number1 = numberData;
        } else {
            _number2 = numberData;
        }
        compute();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return _result; }

    QWidget *embeddedWidget() override { return nullptr; }

    QString resultText() const { return _resultText; }

Q_SIGNALS:
    void resultChanged();

private:
    void compute()
    {
        if (_number1 && _number2) {
            bool val = _number1->number() > _number2->number();
            _result = std::make_shared<BooleanData>(val);
            _resultText = val ? "TRUE" : "FALSE";
        } else {
            _result.reset();
            _resultText = "?";
        }
        Q_EMIT resultChanged();
        Q_EMIT dataUpdated(0);
    }

    std::shared_ptr<DecimalData> _number1;
    std::shared_ptr<DecimalData> _number2;
    std::shared_ptr<BooleanData> _result;
    QString _resultText = "?";
};
