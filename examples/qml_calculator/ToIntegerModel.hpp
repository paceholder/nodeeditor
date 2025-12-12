#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "DecimalData.hpp"
#include "IntegerData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class ToIntegerModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(int resultValue READ resultValue NOTIFY resultChanged)

public:
    QString caption() const override { return QStringLiteral("To Int"); }
    QString name() const override { return QStringLiteral("ToInteger"); }

    unsigned int nPorts(PortType portType) const override
    {
        return 1;
    }

    NodeDataType dataType(PortType portType, PortIndex) const override
    {
        if (portType == PortType::In) {
            return DecimalData{}.type();
        }
        return IntegerData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        auto decimal = std::dynamic_pointer_cast<DecimalData>(data);
        if (decimal) {
            _resultValue = static_cast<int>(decimal->number());
            _result = std::make_shared<IntegerData>(_resultValue);
        } else {
            _resultValue = 0;
            _result.reset();
        }
        Q_EMIT resultChanged();
        Q_EMIT dataUpdated(0);
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return _result; }

    QWidget *embeddedWidget() override { return nullptr; }

    int resultValue() const { return _resultValue; }

Q_SIGNALS:
    void resultChanged();

private:
    int _resultValue = 0;
    std::shared_ptr<IntegerData> _result;
};
