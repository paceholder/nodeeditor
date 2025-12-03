#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class StringData : public NodeData
{
public:
    StringData()
        : _text("")
    {}

    StringData(QString const &text)
        : _text(text)
    {}

    NodeDataType type() const override { return NodeDataType{"string", "String"}; }

    QString text() const { return _text; }

private:
    QString _text;
};
