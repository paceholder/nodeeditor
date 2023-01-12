#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageShowModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    ~ImageShowModel() = default;

public:
    void init() override;

    QString caption() const override { return QString("Image Display"); }

    QString name() const override { return QString("ImageShowModel"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex) override;

    QWidget *embeddedWidget() override;

    bool resizable() const override { return true; }

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *_label = nullptr;
};
