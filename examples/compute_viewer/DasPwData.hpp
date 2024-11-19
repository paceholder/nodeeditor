#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DasPwData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"daspw", "DasPw"}; }
};
