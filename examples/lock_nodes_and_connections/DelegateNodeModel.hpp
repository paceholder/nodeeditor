#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class CameraModelData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"SimpleData", "Simple Data"}; }
};

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class CameraModel : public NodeDelegateModel
{
    Q_OBJECT

public:

    CameraModel() { 
        auto& style = nodeStyle();
        QColor color = style.ConnectionPointColor;
    }

    CameraModel(const QString &name,
                    const QString &caption = "default caption",
                    const QString &category = "default category",
                    int inCount = 1,
                    int outCount = 1)
        : NodeDelegateModel(name, caption, category,inCount,outCount)
    {
        auto &style = nodeStyle();
        QColor color = style.ConnectionPointColor;
    }

public:

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override
    {
        if (portType == PortType::In) {
            return NodeDataType{name(), "Input"};
        } else if (portType == PortType::Out) {
            return NodeDataType{name(), "Output"};
        } else {
            return NodeDataType{"Default", "Default"};
        }
    }

    std::shared_ptr<NodeData> outData(PortIndex const port) override
    {
        return std::make_shared<CameraModelData>();
    }

    bool portCaptionVisible(PortType, PortIndex) const override { return true; }

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
