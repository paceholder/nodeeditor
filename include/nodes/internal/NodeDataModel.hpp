#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "Definitions.hpp"
#include "Export.hpp"
#include "NodeData.hpp"
#include "NodeStyle.hpp"
#include "Serializable.hpp"

namespace QtNodes
{

class StyleCollection;

class NODE_EDITOR_PUBLIC NodeDataModel
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  NodeDataModel();

  virtual
  ~NodeDataModel() = default;

  /// It is possible to hide caption in GUI
  virtual
  bool
  captionVisible() const { return true; }

  /// Caption is used in GUI
  virtual
  QString
  caption() const = 0;

  /// It is possible to hide port caption in GUI
  virtual
  bool
  portCaptionVisible(PortType, PortIndex) const { return false; }

  /// Port caption is used in GUI to label individual ports
  virtual
  QString
  portCaption(PortType, PortIndex) const { return QString(); }

  /// Name makes this model unique
  virtual
  QString
  name() const = 0;

public:

  QJsonObject
  save() const override;

public:

  virtual
  unsigned int
  nPorts(PortType portType) const = 0;

  virtual
  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const = 0;

public:

  virtual
  ConnectionPolicy
  portOutConnectionPolicy(PortIndex) const
  {
    return ConnectionPolicy::Many;
  }


  NodeStyle const &
  nodeStyle() const;

  void
  setNodeStyle(NodeStyle const &style);

public:

  /// Triggers the algorithm
  virtual
  void
  setInData(std::shared_ptr<NodeData> nodeData,
            PortIndex const port) = 0;

  virtual
  std::shared_ptr<NodeData>
  outData(PortIndex const port) = 0;

  virtual
  QWidget *
  embeddedWidget() = 0;

  virtual
  bool
  resizable() const { return false; }

public Q_SLOTS:

  virtual void
  inputConnectionCreated(ConnectionId const &)
  {}


  virtual void
  inputConnectionDeleted(ConnectionId const &)
  {}


  virtual void
  outputConnectionCreated(ConnectionId const &)
  {}


  virtual void
  outputConnectionDeleted(ConnectionId const &)
  {}


Q_SIGNALS:

  void
  dataUpdated(PortIndex const index);

  void
  dataInvalidated(PortIndex const index);

  void
  computingStarted();

  void
  computingFinished();

  void
  embeddedWidgetSizeUpdated();

private:

  NodeStyle _nodeStyle;
};


}
