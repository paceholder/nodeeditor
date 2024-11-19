#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class StringData : public NodeData
{
public:
    StringData()
        : _text()
    {}

    StringData(QString const text)
        : _text(text)
    {}

    NodeDataType type() const override { return NodeDataType{"string", "String"}; }

    QString number() const { return _text; }

    QString numberAsText() const { return _text; }

private:
    QString _text;
};
