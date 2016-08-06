#pragma once

#include <nodes/NodeDataModel>

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class TextData : public NodeData
{
public:

  NodeDataType type() const override
  { return NodeDataType{"text", "Text"}; }

private:

  QString _text;
};
