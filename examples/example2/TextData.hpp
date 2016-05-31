#pragma once

#include <nodes/NodeDataModel>

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class TextData : public NodeData
{
public:

  QString type() const override
  { return "text"; }

  QString name() const override
  { return "Text"; }

private:

  QString _text;
};
