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

/**
 * The class wraps Node-specific data operations and propagates it to
 * the nesting DataFlowGraphModel which is a subclass of
 * AbstractGrapModel.
 * This class is the same what has been called NodeDataModel before v3.
 */
class NODE_EDITOR_PUBLIC NodeDelegateModel
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  NodeDelegateModel();

  virtual
  ~NodeDelegateModel() = default;

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
  portConnectionPolicy(PortType, PortIndex) const;

  NodeStyle const&
  nodeStyle() const;

  void
  setNodeStyle(NodeStyle const& style);

public:

  virtual
  void
  setInData(std::shared_ptr<NodeData> nodeData,
            PortIndex const portIndex) = 0;

  virtual
  std::shared_ptr<NodeData>
  outData(PortIndex const port) = 0;

  /**
   * It is recommented to preform a lazy initialization for the
   * embedded widget and create it inside this function, not in the
   * constructor of the current model.
   *
   * Our Model Registry is able to shortly instantiate models in order
   * to call the non-static `Model::name()`. If the embedded widget is
   * allocated in the constructor but not actually embedded into some
   * QGraphicsProxyWidget, we'll gonna have a dangling pointer.
   */
  virtual
  QWidget*
  embeddedWidget() = 0;

  virtual
  bool
  resizable() const { return false; }

public Q_SLOTS:

  virtual void
  inputConnectionCreated(ConnectionId const&)
  {}


  virtual void
  inputConnectionDeleted(ConnectionId const&)
  {}


  virtual void
  outputConnectionCreated(ConnectionId const&)
  {}


  virtual void
  outputConnectionDeleted(ConnectionId const&)
  {}


Q_SIGNALS:

  /// Triggers the updates in the nodes downstream.
  void
  dataUpdated(PortIndex const index);

  /// Triggers the propagation of the empty data downstream.
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
