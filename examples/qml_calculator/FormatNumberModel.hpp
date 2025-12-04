#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "DecimalData.hpp"
#include "StringData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class FormatNumberModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(QString formatPattern READ formatPattern WRITE setFormatPattern NOTIFY formatPatternChanged)
    Q_PROPERTY(QString formattedText READ formattedText NOTIFY formattedTextChanged)

public:
    FormatNumberModel()
        : _formatPattern("Result: %1")
    {}

    QString caption() const override { return QStringLiteral("Format"); }
    QString name() const override { return QStringLiteral("FormatNumber"); }

    unsigned int nPorts(PortType portType) const override
    {
        return 1;
    }

    NodeDataType dataType(PortType portType, PortIndex) const override
    {
        if (portType == PortType::In) {
            return DecimalData{}.type();
        }
        return StringData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        _inputNumber = std::dynamic_pointer_cast<DecimalData>(data);
        compute();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return _result; }

    QWidget *embeddedWidget() override { return nullptr; }

    QString formatPattern() const { return _formatPattern; }
    
    void setFormatPattern(const QString &pattern)
    {
        if (_formatPattern != pattern) {
            _formatPattern = pattern;
            Q_EMIT formatPatternChanged();
            compute();
        }
    }

    QString formattedText() const { return _formattedText; }

Q_SIGNALS:
    void formatPatternChanged();
    void formattedTextChanged();

private:
    void compute()
    {
        if (_inputNumber) {
            _formattedText = _formatPattern.arg(_inputNumber->number(), 0, 'f', 2);
            _result = std::make_shared<StringData>(_formattedText);
        } else {
            _formattedText = "";
            _result.reset();
        }
        Q_EMIT formattedTextChanged();
        Q_EMIT dataUpdated(0);
    }

    QString _formatPattern;
    QString _formattedText;
    std::shared_ptr<DecimalData> _inputNumber;
    std::shared_ptr<StringData> _result;
};
