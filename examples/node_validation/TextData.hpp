#pragma once

#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// Simple text data class for transferring strings between nodes
class TextData : public NodeData
{
public:
    TextData() = default;

    TextData(QString const &text)
        : _text(text)
    {}

    NodeDataType type() const override { return NodeDataType{"text", "Text"}; }

    QString text() const { return _text; }

    bool isEmpty() const { return _text.isEmpty(); }

private:
    QString _text;
};
