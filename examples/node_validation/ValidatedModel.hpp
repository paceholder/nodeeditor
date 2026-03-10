#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "TextData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeProcessingStatus;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// A node that demonstrates validation states and processing status.
/// It validates that input text has a minimum length and simulates processing.
class ValidatedModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    ValidatedModel()
        : _widget(nullptr)
        , _label(nullptr)
        , _minLength(3)
    {}

    QString caption() const override { return QStringLiteral("Text Validator"); }

    QString name() const override { return QStringLiteral("TextValidator"); }

    unsigned int nPorts(PortType portType) const override
    {
        if (portType == PortType::In)
            return 1;
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
        return _outputData;
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        auto textData = std::dynamic_pointer_cast<TextData>(data);

        if (!textData || textData->isEmpty()) {
            // No input - set Empty status
            setNodeProcessingStatus(NodeProcessingStatus::Empty);
            NodeValidationState state;
            state._state = NodeValidationState::State::Warning;
            state._stateMessage = "No input data";
            setValidationState(state);

            _outputData.reset();
            if (_label)
                _label->setText("Waiting for input...");

            Q_EMIT dataInvalidated(0);
            return;
        }

        // We have input - start "processing"
        setNodeProcessingStatus(NodeProcessingStatus::Processing);
        if (_label)
            _label->setText("Processing...");

        // Simulate async processing with a timer
        QTimer::singleShot(500, this, [this, textData]() {
            processInput(textData);
        });
    }

    QWidget *embeddedWidget() override
    {
        if (!_widget) {
            _widget = new QWidget();
            auto layout = new QVBoxLayout(_widget);
            layout->setContentsMargins(5, 5, 5, 5);

            _label = new QLabel("Waiting for input...");
            _label->setWordWrap(true);
            _label->setMinimumWidth(120);
            layout->addWidget(_label);
        }
        return _widget;
    }

private:
    void processInput(std::shared_ptr<TextData> textData)
    {
        QString text = textData->text();

        if (text.length() < _minLength) {
            // Validation failed
            NodeValidationState state;
            state._state = NodeValidationState::State::Error;
            state._stateMessage = QString("Text must be at least %1 characters").arg(_minLength);
            setValidationState(state);

            setNodeProcessingStatus(NodeProcessingStatus::Failed);

            _outputData.reset();
            if (_label)
                _label->setText(QString("Error: too short\n(min %1 chars)").arg(_minLength));

            Q_EMIT dataInvalidated(0);
        } else if (text.length() < _minLength * 2) {
            // Partial success - warning
            NodeValidationState state;
            state._state = NodeValidationState::State::Warning;
            state._stateMessage = "Text is short but acceptable";
            setValidationState(state);

            setNodeProcessingStatus(NodeProcessingStatus::Partial);

            _outputData = std::make_shared<TextData>(text.toUpper());
            if (_label)
                _label->setText("Output: " + _outputData->text());

            Q_EMIT dataUpdated(0);
        } else {
            // Full success
            NodeValidationState state;
            state._state = NodeValidationState::State::Valid;
            state._stateMessage = "";
            setValidationState(state);

            setNodeProcessingStatus(NodeProcessingStatus::Updated);

            _outputData = std::make_shared<TextData>(text.toUpper());
            if (_label)
                _label->setText("Output: " + _outputData->text());

            Q_EMIT dataUpdated(0);
        }
    }

private:
    std::shared_ptr<TextData> _outputData;
    QWidget *_widget;
    QLabel *_label;
    int _minLength;
};
