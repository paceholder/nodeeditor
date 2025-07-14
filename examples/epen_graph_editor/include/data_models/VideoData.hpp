#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class VideoData : public NodeData
{
public:
    VideoData()
        : _number(0.0)
    {}

    VideoData(double const number)
        : _number(number)
    {}

    NodeDataType type() const override { return NodeDataType{"decimal", "Decimal"}; }

    double number() const { return _number; }

    QString numberAsText() const { return QString::number(_number, 'f'); }

private:
    double _number;
};
