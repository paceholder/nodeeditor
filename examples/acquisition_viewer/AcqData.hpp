#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class AcqData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"acquisition", ""}; }
};
