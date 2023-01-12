#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "Definitions.hpp"
#include "Export.hpp"
#include "NodeData.hpp"
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
    QJsonObject save() const override;

    void load(QJsonObject const &) override;

public:
    NodeStyle const &nodeStyle() const;

    void setNodeStyle(NodeStyle const &style);

public:
    virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex) = 0;

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

#pragma region /* port */

public:
    unsigned int nPorts(PortType portType) const;

    void createPort(PortType portType,
                    std::shared_ptr<NodeData> nodeData,
                    const PortCaption name = "",
                    ConnectionPolicy policy = ConnectionPolicy::One);

    NodePort const &port(PortType portType, PortIndex portIndex) const;

    template<class T>
    std::shared_ptr<T> portData(PortType portType, PortIndex portIndex) const
    {
        return std::dynamic_pointer_cast<T>(portData(portType, portIndex));
    }

    std::shared_ptr<NodeData> portData(PortType portType, PortIndex portIndex) const;

    void setPortData(PortType portType, PortIndex portIndex, std::shared_ptr<NodeData> nodeData);

    /// Port caption is used in GUI to label individual ports
    PortCaption portCaption(PortType portType, PortIndex portIndex) const;

    void setPortCaption(PortType portType, PortIndex portIndex, const PortCaption name);

    ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex portIndex) const;

    void setPortConnectionPolicy(PortType portType, PortIndex portIndex, ConnectionPolicy policy);

#pragma endregion /* port */

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

    /// Call this function before deleting the data associated with ports.
    /**
   * The function notifies the Graph Model and makes it remove and recompute the
   * affected connection addresses.
   */
    void portsAboutToBeDeleted(PortType const portType, PortIndex const first, PortIndex const last);

    /// Call this function when data and port moditications are finished.
    void portsDeleted();

    /// Call this function before inserting the data associated with ports.
    /**
   * The function notifies the Graph Model and makes it recompute the affected
   * connection addresses.
   */
    void portsAboutToBeInserted(PortType const portType,
                                PortIndex const first,
                                PortIndex const last);

    /// Call this function when data and port moditications are finished.
    void portsInserted();

private:
    NodeStyle _nodeStyle;

    std::vector<NodePort> _inputPorts;
    std::vector<NodePort> _outputPorts;
};

} // namespace QtNodes
