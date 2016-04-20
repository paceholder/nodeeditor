#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

#include "PortType.hpp"

#include "NodeData.hpp"

class NodeDataModel : public QObject
{

public:

  virtual ~NodeDataModel() {}

public:

  virtual unsigned int nSlots(PortType portType) const = 0;

  virtual
  std::shared_ptr<NodeData>
  data(PortType portType, int slot) = 0;

  /// Triggers the algorithm
  virtual void setInputData(std::shared_ptr<NodeData> nodeData, int slot) = 0;

  virtual QWidget * embeddedWidget() = 0;

signals:

  virtual void computingStarted()  = 0;
  virtual void computingFinished() = 0;
};
