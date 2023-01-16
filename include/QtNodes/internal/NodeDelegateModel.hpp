#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "Definitions.hpp"
#include "Export.hpp"
#include "NodeStyle.hpp"
#include "Serializable.hpp"

namespace QtNodes {

class StyleCollection;

/**
 * The class wraps Node-specific data operations and propagates it to
 * the nesting DataFlowGraphModel which is a subclass of
 * AbstractGrapModel.
 * This class is the same what has been called NodeDataModel before v3.
 */
class NODE_EDITOR_PUBLIC NodeDelegateModel : public QObject, public Serializable
{
    Q_OBJECT

public:
    NodeDelegateModel();

    virtual ~NodeDelegateModel() = default;

    virtual void init() = 0;

    /// Caption is used in GUI
    virtual QString caption() const { return ""; }

    /// Name makes this model unique
    virtual QString name() const = 0;

public:
    NodeStyle const &nodeStyle() const { return _nodeStyle; }

    void setNodeStyle(NodeStyle const &style) { _nodeStyle = style; }

public:
    virtual void setInData(QVariant const nodeData, PortIndex const portIndex) = 0;

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
    virtual QWidget *embeddedWidget() = 0;

    virtual bool resizable() const { return false; }

    virtual bool isDynamicPorts() const { return false; }

public Q_SLOTS:

    virtual void inputConnectionCreated(ConnectionId const &) {}

    virtual void inputConnectionDeleted(ConnectionId const &) {}

    virtual void outputConnectionCreated(ConnectionId const &) {}

    virtual void outputConnectionDeleted(ConnectionId const &) {}

Q_SIGNALS:

    /// Triggers the updates in the nodes downstream.
    void dataUpdated(PortIndex const index);

    /// Triggers the propagation of the empty data downstream.
    void dataInvalidated(PortIndex const index);

    void computingStarted();

    void computingFinished();

    void embeddedWidgetSizeUpdated();

    void createPort(PortType portType,
                    const NodeDataType dataType,
                    const PortCaption name = "",
                    ConnectionPolicy policy = ConnectionPolicy::One);

    void insertPort(PortType portType,
                    PortIndex portIndex,
                    const NodeDataType dataType,
                    const PortCaption name = "",
                    ConnectionPolicy policy = ConnectionPolicy::One);

    void removePort(PortType portType, PortIndex portIndex);

    void updatePortCaption(PortType portType, PortIndex portIndex, const PortCaption name);

    void updatePortConnectionPolicy(PortType portType, PortIndex portIndex, ConnectionPolicy policy);

    void updateOutPortData(PortIndex portIndex, QVariant nodeData);

private:
    NodeStyle _nodeStyle;
};

} // namespace QtNodes
