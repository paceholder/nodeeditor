#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QLineEdit>

#include "TextData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// A simple text source node with an embedded line edit
class TextSourceModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextSourceModel()
        : _lineEdit(nullptr)
    {}

    ~TextSourceModel() override { delete _lineEdit; }

    QString caption() const override { return QStringLiteral("Text Source"); }

    QString name() const override { return QStringLiteral("TextSource"); }

    unsigned int nPorts(PortType portType) const override
    {
        if (portType == PortType::Out)
            return 1;
        return 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return TextData{}.type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return _data;
    }

    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}

    QWidget *embeddedWidget() override
    {
        if (!_lineEdit) {
            _lineEdit = new QLineEdit();
            _lineEdit->setPlaceholderText("Enter text...");
            connect(_lineEdit, &QLineEdit::textChanged, this, &TextSourceModel::onTextChanged);
        }
        return _lineEdit;
    }

private Q_SLOTS:
    void onTextChanged(QString const &text)
    {
        _data = std::make_shared<TextData>(text);
        Q_EMIT dataUpdated(0);
    }

private:
    std::shared_ptr<TextData> _data;
    QPointer<QLineEdit> _lineEdit;
};
