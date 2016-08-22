#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "PortType.hpp"

#include "NodeData.hpp"

class NodeDataModel : public QObject
{
  Q_OBJECT

public:

  virtual ~NodeDataModel() {}

public:

  virtual unsigned int nPorts(PortType portType) const = 0;

  virtual
  NodeDataType dataType(PortType portType, PortIndex portIndex) const = 0;

  virtual
  std::shared_ptr<NodeData>outData(PortIndex port) = 0;

  /// Triggers the algorithm
  virtual void setInData(std::shared_ptr<NodeData> nodeData,
                         PortIndex port) = 0;

  virtual QWidget * embeddedWidget() = 0;

signals:

  void dataUpdated(PortIndex index);
  void dataInvalidated(PortIndex index);

  void computingStarted();
  void computingFinished();
};
