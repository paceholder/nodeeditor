#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>

#include <iostream>

class DecimalData;

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLineEdit;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberSourceDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    NumberSourceDataModel();
    NumberSourceDataModel(const QString &name,
                   const QString &caption = "default caption",
                   const QString &category = "default category",
                   unsigned int inCount = 1,
                   unsigned int outCount = 1): NodeDelegateModel(name, caption, category,inCount,outCount){}
    virtual ~NumberSourceDataModel() {}

public:

    bool captionVisible() const override { return false; }

public:
    QJsonObject save() const override;

    void load(QJsonObject const &p) override;

public:
    unsigned int nPorts(PortType portType) const;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}

    QWidget *embeddedWidget() override;

public:
    void setNumber(double number);

private Q_SLOTS:

    void onTextEdited(QString const &string);

private:
    std::shared_ptr<DecimalData> _number;

    QLineEdit *_lineEdit;
};
