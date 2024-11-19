#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class ClutterFilterData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"clutterfilter", "ClutterFilter"}; }
};
