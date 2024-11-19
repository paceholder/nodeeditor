#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class InputData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"input", "Input"}; }
};
