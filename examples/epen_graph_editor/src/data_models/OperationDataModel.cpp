#include "data_models/OperationDataModel.hpp"

OperationDataModel::OperationDataModel()
{
    _name = "NewNode001";
}

unsigned int OperationDataModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType OperationDataModel::dataType(PortType, PortIndex) const
{
    return IMAGE_DATA_TYPE;
}

void OperationDataModel::deselected() {}

void OperationDataModel::setNodeName(QString name)
{
    _name = name;
}

std::shared_ptr<NodeData> OperationDataModel::outData(PortIndex port)
{
    return nullptr;
};

void OperationDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {

};