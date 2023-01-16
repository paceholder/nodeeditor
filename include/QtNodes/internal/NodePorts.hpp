#pragma once

#include "Definitions.hpp"

namespace QtNodes {

class NodePorts // : public QObject
{
    // Q_OBJECT

public:
    NodePorts();
    ~NodePorts();

public:
    unsigned int nPorts(PortType portType) const;

    NodePort const &port(PortType portType, PortIndex portIndex) const;

    void createPort(PortType portType,
                    std::shared_ptr<NodeData> nodeData,
                    const PortCaption name = "",
                    ConnectionPolicy policy = ConnectionPolicy::One);

    /// @brief Insert new port at `portIndex`
    void insertPort(PortType portType,
                    PortIndex portIndex,
                    std::shared_ptr<NodeData> nodeData,
                    const PortCaption name = "",
                    ConnectionPolicy policy = ConnectionPolicy::One);

    void removePort(PortType portType, PortIndex portIndex);

    template<class T>
    std::shared_ptr<T> portData(PortType portType, PortIndex portIndex) const
    {
        return std::dynamic_pointer_cast<T>(portData(portType, portIndex));
    }

    std::shared_ptr<NodeData> portData(PortType portType, PortIndex portIndex) const;

    void setPortData(PortType portType, PortIndex portIndex, std::shared_ptr<NodeData> nodeData);

    /// Port caption is used in GUI to label individual ports
    PortCaption portCaption(PortType portType, PortIndex portIndex) const;

    void setPortCaption(PortType portType, PortIndex portIndex, const PortCaption name);

    ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex portIndex) const;

    void setPortConnectionPolicy(PortType portType, PortIndex portIndex, ConnectionPolicy policy);

private:
    std::vector<NodePort> _inputPorts;
    std::vector<NodePort> _outputPorts;
};

} // namespace QtNodes