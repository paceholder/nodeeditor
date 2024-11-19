#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DopplerViewerData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"dopplerviewer", "DopplerViewer"}; }
};
