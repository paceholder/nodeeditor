#pragma once

#include "StringData.hpp"
#include <memory>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class StringDataModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    StringDataModel() = default;
    virtual QString caption() const { return _text->numberAsText(); }
    virtual QString name() const { return "Text"; };
    virtual unsigned int nPorts(PortType portType) const
    {
        unsigned int result = 1;

        switch (portType) {
        case PortType::In:
            result = 0;
            break;

        case PortType::Out:
            result = 1;
            break;

        default:
            break;
        }

        return result;
    }

    virtual NodeDataType dataType(PortType portType, PortIndex portIndex) const
    {
        switch (portType) {
        case PortType::In:
            return StringData().type();

        case PortType::Out:
            return StringData().type();
        case PortType::None:
        default:
            return {};
        }
    }
    virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex) {}

    virtual std::shared_ptr<NodeData> outData(PortIndex const port)
    {
        return std::static_pointer_cast<NodeData>(_text);
    }
    virtual QWidget *embeddedWidget() { return nullptr; }

private:
    std::shared_ptr<StringData> _text = std::make_shared<StringData>("Hello");
};
