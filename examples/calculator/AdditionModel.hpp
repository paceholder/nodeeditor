#pragma once

#include <QtCore/QObject>

#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class AdditionModel : public MathOperationDataModel
{
public:

  virtual
  ~AdditionModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("Addition"); }

  QString
  name() const override
  { return QStringLiteral("Addition"); }

private:

  void
  compute() override
  {
    PortIndex const outPortIndex = 0;

    auto n1 = _number1.lock();
    auto n2 = _number2.lock();

    if (n1 && n2)
    {
      modelValidationState = NodeValidationState::Valid;
      modelValidationError = QString();
      _result = std::make_shared<DecimalData>(n1->number() +
                                              n2->number());
    }
    else
    {
      modelValidationState = NodeValidationState::Warning;
      modelValidationError = QStringLiteral("Missing or incorrect inputs");
      _result.reset();
    }

    Q_EMIT dataUpdated(outPortIndex);
  }
};

class MultiAdditionModel : public NodeDataModel
{
public:
    QString
        caption() const override
    {
        return QStringLiteral("Multi Addition");
    }

    QString
        name() const override
    {
        return QStringLiteral("Multi Addition");
    }

    unsigned int
        nPorts(PortType portType) const override
    {
        return 1;
    }

    NodeDataType
        dataType(PortType portType,
            PortIndex portIndex) const
    {
        return DecimalData().type();
    }

    std::shared_ptr<NodeData>
        outData(PortIndex port) override
    {
        return _result;
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override
    {
    }

    void setInData(std::vector<std::shared_ptr<NodeData>> data, PortIndex portIndex) override
    {
        _input.clear();
        _input.reserve(data.size());
        for (auto& node : data)
        {
            std::shared_ptr<DecimalData> decimalData = std::dynamic_pointer_cast<DecimalData>(node);
            if (decimalData != nullptr)
                _input.push_back(decimalData);
        }

        if (_input.empty())
        {
            modelValidationState = NodeValidationState::Warning;
        }
        else
        {
            modelValidationState = NodeValidationState::Valid;
        }

        double result = 0.0f;
        for (auto& node : _input)
        {
            std::shared_ptr<DecimalData> locked = node.lock();
            result += locked->number();
        }

        _result = std::make_shared<DecimalData>(result);
        dataUpdated(0);
    }

    QWidget *
        embeddedWidget() override { return nullptr; }

    NodeValidationState
        validationState() const override
    {
        return modelValidationState;
    }

    QString
        validationMessage() const override
    {
        return modelValidationError;
    }

    ConnectionPolicy portInConnectionPolicy(PortIndex) const override { return QtNodes::NodeDataModel::ConnectionPolicy::Many; }

private:
    std::vector<std::weak_ptr<DecimalData>> _input;

    std::shared_ptr<DecimalData> _result;

    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = QString("Missing or incorrect inputs");
};
