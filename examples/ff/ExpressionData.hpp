#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ExpressionData : public NodeData
{
public:

  ExpressionData()
    : _expression()
  {}

  ExpressionData(QString const  & text)
    : _expression(text)
  {}

  NodeDataType
  type() const override
  {
    return NodeDataType {"Expression",
                         "Expression"};
  }

  QString const &
  expression() const
  { return _expression; }

private:

  QString _expression;
};
