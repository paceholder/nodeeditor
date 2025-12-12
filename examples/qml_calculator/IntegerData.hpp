#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class IntegerData : public NodeData
{
public:
    IntegerData()
        : _value(0)
    {}

    IntegerData(int value)
        : _value(value)
    {}

    NodeDataType type() const override { return NodeDataType{"integer", "Integer"}; }

    int value() const { return _value; }

private:
    int _value;
};
