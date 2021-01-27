#pragma once


#include <QtWidgets/QWidget>

#include "PortType.hpp"
#include "NodeData.hpp"
#include "Serializable.hpp"
#include "NodeGeometry.hpp"
#include "NodeStyle.hpp"
#include "NodePainterDelegate.hpp"
#include "Export.hpp"
#include "memory.hpp"

namespace QtNodes
{

enum class NodeValidationState
{
  Valid,
  Warning,
  Error
};

/**
 * @brief The NodeProcessingStatus enum defines a node's state in the data topology.
 * It should be used when managing the topology's data flow and should be propagated
 * to subsequent nodes before and after each computation.
 */
enum class NodeProcessingStatus
{
  NoStatus   = 0,
  Updated    = 1,
  Processing = 2,
  Pending    = 3,
  Empty      = 4,
  Failed     = 5,
  Partial    = 6,
};

class Connection;

class StyleCollection;

/**
 * @brief The NodeDataModel class represents the data types of input and output
 * ports of a node. It is used to handle the data flow between nodes and checking
 * the compatibility between each connection's endpoint.
 */
class NODE_EDITOR_PUBLIC NodeDataModel
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  NodeDataModel();

  virtual
  ~NodeDataModel() = default;

  /// Caption is used in GUI
  virtual QString
  caption() const = 0;

  /// Progress is used in GUI
  virtual QString
  progressValue() const = 0;

  /// Nicknames can be assigned to nodes and shown in GUI
  QString
  nickname() const
  {
    return _nickname;
  };

  /// It is possible to hide the nickname in GUI
  virtual bool
  nicknameVisible() const
  {
    return false;
  }

  /// It is possible to hide caption in GUI
  virtual bool
  captionVisible() const
  {
    return true;
  }

  /// Port caption is used in GUI to label individual ports
  virtual QString
  portCaption(PortType, PortIndex) const
  {
    return QString();
  }

  /// It is possible to hide port caption in GUI
  virtual bool
  portCaptionVisible(PortType, PortIndex) const
  {
    return false;
  }

  /// Name makes this model unique
  virtual QString
  name() const = 0;

public:

  QJsonObject
  save() const override;

public:

  virtual
  unsigned int nPorts(PortType portType) const = 0;

  virtual
  NodeDataType dataType(PortType portType, PortIndex portIndex) const = 0;

public:

  enum class ConnectionPolicy
  {
    One,
    Many,
  };

  virtual
  ConnectionPolicy
  portOutConnectionPolicy(PortIndex) const
  {
    return ConnectionPolicy::Many;
  }

  NodeStyle const&
  nodeStyle() const;

  void
  setNodeStyle(NodeStyle const& style);

public:

  /// Triggers the algorithm
  virtual
  void
  setInData(std::shared_ptr<NodeData> nodeData,
            PortIndex port) = 0;

  virtual
  std::shared_ptr<NodeData>
  outData(PortIndex port) = 0;

  virtual
  QWidget *
  embeddedWidget() = 0;

  virtual
  bool
  resizable() const
  {
    return false;
  }

  virtual
  NodeValidationState
  validationState() const
  {
    return NodeValidationState::Valid;
  }

  virtual
  QString
  validationMessage() const
  {
    return QString("");
  }

  virtual
  NodePainterDelegate* painterDelegate() const
  {
    return nullptr;
  }

  /**
   * @brief Returns the node's current processing status.
   */
  virtual
  NodeProcessingStatus
  processingStatus() const
  {
    return NodeProcessingStatus::NoStatus;
  }

  /**
   * @brief Sets the node's nickname.
   * @param nickname New nickname
   */
  virtual void
  setNickname(const QString& nickname)
  {
    _nickname = nickname;
  }

public Q_SLOTS:

  virtual void
  inputConnectionCreated(Connection const&)
  {
  }

  virtual void
  inputConnectionDeleted(Connection const&)
  {
  }

  virtual void
  outputConnectionCreated(Connection const&)
  {
  }

  virtual void
  outputConnectionDeleted(Connection const&)
  {
  }

Q_SIGNALS:

  void
  dataUpdated(PortIndex index);

  void
  dataInvalidated(PortIndex index);

  void
  computingStarted();

  void
  computingFinished();

  void embeddedWidgetSizeUpdated();

protected:
  /**
   * @brief Nickname of the node, which can be edited by the user and is
   * shown as the node title if visible.
   */
  QString _nickname;

private:

  NodeStyle _nodeStyle;

};
}
