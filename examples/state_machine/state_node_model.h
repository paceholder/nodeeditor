#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::PortIndex;
using QtNodes::PortType;
using QtNodes::NodeDataType;

class StateNodeModel : public QtNodes::NodeDataModel
{
public:
    static NodeDataType getTranstitionType();

    QString caption() const override;
    QString name() const override;

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    ConnectionPolicy portOutConnectionPolicy(PortIndex) const override;
    ConnectionPolicy portInConnectionPolicy(PortIndex) const override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    void setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port) override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    QWidget* embeddedWidget() override { return nullptr; }
};