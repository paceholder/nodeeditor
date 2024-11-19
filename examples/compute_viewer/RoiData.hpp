#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class RoiData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"roi", "Roi"}; }
};
