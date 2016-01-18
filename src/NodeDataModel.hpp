#ifndef NODE_DATA_MODEL_HPP
#define NODE_DATA_MODEL_HPP

#include <memory>

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

#include "EndType.hpp"

class NodeDataModel : QObject
{
  Q_OBJECT

public:

  virtual unsigned int nSlots(EndType end) const = 0;

  virtual DataType dataType(EndType end, int slot) = 0;

  /// Triggers the algorithm
  virtual void setInputData(std::shared_ptr<NodeData> nodeData, int slot) = 0;

  virtual QWidget * embeddedWidget() = 0;

signals:

  virtual void computingStarted()  = 0;
  virtual void computingFinished() = 0;
};

#endif //  NODE_DATA_MODEL_HPP
