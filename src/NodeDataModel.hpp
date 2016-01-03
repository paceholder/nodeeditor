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

  unsigned int nSlots(EndType end) const;

  DataType dataType(EndType end, int slot);

  /// Triggers the algorithm
  void setInputData(std::shared_ptr<NodeData> nodeData);

  QWidget * embeddedWidget();

signals:

  void computingStarted();
  void computingFinished();
};

#endif //  NODE_DATA_MODEL_HPP
