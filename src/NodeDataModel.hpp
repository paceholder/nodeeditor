#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "PortType.hpp"
#include "NodeData.hpp"

#include "Export.hpp"

class NODE_EDITOR_PUBLIC NodeDataModel
    : public QObject
{
  Q_OBJECT

public:

  virtual ~NodeDataModel() {}

  virtual QString modelName() const { return QString(); }

public:

  virtual unsigned int nPorts(PortType portType) const = 0;

  virtual
  NodeDataType dataType(PortType portType, PortIndex portIndex) const = 0;

public:

  /// Triggers the algorithm
  virtual void setInData(std::shared_ptr<NodeData> nodeData,
                         PortIndex port) = 0;

  virtual
  std::shared_ptr<NodeData> outData(PortIndex port) = 0;

  virtual QWidget * embeddedWidget() = 0;

signals:

  void dataUpdated(PortIndex index);
  void dataInvalidated(PortIndex index);

  void computingStarted();
  void computingFinished();
};
