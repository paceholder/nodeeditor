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

    NodeDelegateModel(const QString &name,
                      const QString &caption = "default caption",
                      const QString &category = "default category",
                      unsigned int inCount = 1,
                      unsigned int outCount = 1);

    virtual ~NodeDelegateModel() = default;

    /// It is possible to hide caption in GUI
    virtual bool captionVisible() const { return true; }

    /// 获取标题，Caption is used in GUI
    QString caption() const;

    /// It is possible to hide port caption in GUI
    virtual bool portCaptionVisible(PortType, PortIndex) const { return false; }

    /// Port caption is used in GUI to label individual ports
    virtual QString portCaption(PortType, PortIndex) const { return QString(); }

    /// 获取模型唯一名称，Name makes this model unique
    QString name() const;

    /// @brief 获取目录名称
    /// @return 
    QString category() const;

public:
    QJsonObject save() const override;

    void load(QJsonObject const &) override;

public:
    unsigned int nPorts(PortType portType) const;

    virtual NodeDataType dataType(PortType portType, PortIndex portIndex) const = 0;

public:
    /// @brief 设置端口的单连、多连策略
    /// @param  端口类型
    /// @param  端口索引
    /// @return 连接策略
    virtual ConnectionPolicy portConnectionPolicy(PortType, PortIndex) const;

    NodeStyle const &nodeStyle() const;

    void setNodeStyle(NodeStyle const &style);

public:
    virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex) = 0;

    virtual std::shared_ptr<NodeData> outData(PortIndex const port) = 0;

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
    
    /// @brief 标题
    QString _caption;

    /// @brief 唯一名称
    QString _name;

    /// @brief 目录
    QString _category;

    /// @brief 输入端口数量
    unsigned int _inCount;

    /// @brief 输出端口数量
    unsigned int _outCount;

    /// @brief 
    mutable std::unordered_map<PortId, QList<PortTransDataType>> _nodePortTransTypes;
};

} // namespace QtNodes
