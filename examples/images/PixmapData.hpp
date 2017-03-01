#pragma once

#include <QtGui/QPixmap>

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class PixmapData : public NodeData
{
public:

  PixmapData() {}

  PixmapData(QPixmap const &pixmap)
    : _pixmap(pixmap)
  {}

  NodeDataType
  type() const override
  {
    //       id      name
    return {"pixmap", "P"};
  }

  QPixmap
  pixmap() const { return _pixmap; }

private:

  QPixmap _pixmap;
};
