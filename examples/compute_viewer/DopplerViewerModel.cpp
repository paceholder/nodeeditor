#include "DopplerViewerModel.hpp"
#include "ClutterFilterData.hpp"
#include "DopplerViewerData.hpp"
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int DopplerViewerModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 0;

    return result;
}

NodeDataType DopplerViewerModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return ClutterFilterData().type();
    } else {
        return DopplerViewerData().type();
    }
}

std::shared_ptr<NodeData> DopplerViewerModel::outData(PortIndex)
{
    return nullptr;
}

void DopplerViewerModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *DopplerViewerModel::embeddedWidget()
{
    return nullptr;
}
