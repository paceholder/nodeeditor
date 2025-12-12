#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class BooleanData : public NodeData
{
public:
    BooleanData()
        : _value(false)
    {}

    BooleanData(bool value)
        : _value(value)
    {}

    NodeDataType type() const override { return NodeDataType{"boolean", "Boolean"}; }

    bool value() const { return _value; }

private:
    bool _value;
};
