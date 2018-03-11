#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class TextData : public NodeData
{
public:

  TextData() {}

  TextData(QString const &text)
    : _text(text)
  {}

  NodeDataType type() const override
  {
    NodeDataType data;
    data.id = "text";
    data.name = "Text";
    return data;
  }

  QString text() const { return _text; }

private:

  QString _text;
};
