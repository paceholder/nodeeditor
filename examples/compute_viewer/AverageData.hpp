#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class AverageX1Data : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"average", "x1"}; }
};

class AverageX2Data : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"average", "x2"}; }
};

class AverageData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"average", "x"}; }
};
