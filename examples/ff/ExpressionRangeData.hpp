#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ExpressionRangeData : public NodeData
{
public:

  ExpressionRangeData()
  {}

  ExpressionRangeData(QString const  & text,
                      std::vector<double> const &range)
    : _expression(text)
    , _range(range)
  {}

  NodeDataType
  type() const override
  {
    return NodeDataType {"ExpressionRange",
                         "E"};
  }

  QString const &
  expression() const
  { return _expression; }

  std::vector<double> const &
  range() const
  { return _range; }

private:

  QString _expression;

  std::vector<double> _range;
};
