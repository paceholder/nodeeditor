#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class CalculatorData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"calculator", "Calculator"}; }
};
