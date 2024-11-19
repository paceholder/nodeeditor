#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DecimationData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"decimation", "Decimation"}; }
};
