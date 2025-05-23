#pragma once

#include <QtCore/QObject>

#include "TextData.hpp"

#include <QtNodes/NodeDelegateModel>

#include <iostream>

using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLineEdit;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextSourceDataModel();
    TextSourceDataModel(const QString &name,
                      const QString &caption = "default caption",
                      const QString &category = "default category",
                      unsigned int inCount = 1,
                        unsigned int outCount = 1);

public:

    bool captionVisible() const override { return false; }

    static QString Name() { return QString("TextSourceDataModel"); }

public:
    unsigned int nPorts(PortType portType) const;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override;

private Q_SLOTS:

    void onTextEdited(QString const &string);

private:
    QLineEdit *_lineEdit;
};
