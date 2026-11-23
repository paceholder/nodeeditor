#pragma once

#include <QtNodes/Definitions>
#include <QtNodes/NodeDelegateModel>

#include <memory>
#include <utility>

/// A node delegate model with no data and no widget.
/// The tests set the name, the caption and the port counts.
/// Subclass it to add small extras. It has no static Name(), so register it
/// with a creator function, or through a subclass with a default constructor.
class StubDelegateModel : public QtNodes::NodeDelegateModel
{
public:
    StubDelegateModel(QString name,
                      QString caption,
                      unsigned int nInPorts = 0,
                      unsigned int nOutPorts = 0)
        : _name(std::move(name))
        , _caption(std::move(caption))
        , _nInPorts(nInPorts)
        , _nOutPorts(nOutPorts)
    {}

    QString name() const override { return _name; }
    QString caption() const override { return _caption; }

    unsigned int nPorts(QtNodes::PortType portType) const override
    {
        switch (portType) {
        case QtNodes::PortType::In:
            return _nInPorts;
        case QtNodes::PortType::Out:
            return _nOutPorts;
        default:
            return 0;
        }
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override
    {
        return {};
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return nullptr; }

    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}

    QWidget *embeddedWidget() override { return nullptr; }

private:
    QString _name;
    QString _caption;
    unsigned int _nInPorts;
    unsigned int _nOutPorts;
};
