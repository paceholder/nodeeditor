#include "BasicGraphicsScene.hpp"

#include "AbstractNodeGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "DefaultHorizontalNodeGeometry.hpp"
#include "DefaultNodePainter.hpp"
#include "DefaultVerticalNodeGeometry.hpp"
#include "GraphicsView.hpp"
#include "NodeGraphicsObject.hpp"
#include "QtNodes/InvalidData.hpp"
#include "WidgetHorizontalNodeGeometry.hpp"

#include <QUndoStack>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtGlobal>

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <queue>
#include <QKeyEvent>

namespace QtNodes {

    BasicGraphicsScene::BasicGraphicsScene(AbstractGraphModel &graphModel, QObject *parent)
            : QGraphicsScene(parent), _graphModel(graphModel),
              _nodeGeometry(std::make_unique<DefaultHorizontalNodeGeometry>(_graphModel)),
              _nodePainter(std::make_unique<DefaultNodePainter>()), _nodeDrag(false), _undoStack(new QUndoStack(this)),
              _orientation(Qt::Horizontal) {
        setItemIndexMethod(QGraphicsScene::NoIndex);

        connect(&_graphModel,
                &AbstractGraphModel::connectionCreated,
                this,
                &BasicGraphicsScene::onConnectionCreated);

        connect(&_graphModel,
                &AbstractGraphModel::connectionDeleted,
                this,
                &BasicGraphicsScene::onConnectionDeleted);

        connect(&_graphModel,
                &AbstractGraphModel::nodeCreated,
                this,
                &BasicGraphicsScene::onNodeCreated);

        connect(&_graphModel,
                &AbstractGraphModel::nodeDeleted,
                this,
                &BasicGraphicsScene::onNodeDeleted);

        connect(&_graphModel,
                &AbstractGraphModel::nodePositionUpdated,
                this,
                &BasicGraphicsScene::onNodePositionUpdated);

        connect(&_graphModel,
                &AbstractGraphModel::nodeUpdated,
                this,
                &BasicGraphicsScene::onNodeUpdated);

        connect(this, &BasicGraphicsScene::nodeClicked, this, &BasicGraphicsScene::onNodeClicked);

        connect(&_graphModel, &AbstractGraphModel::modelReset, this, &BasicGraphicsScene::onModelReset);

        traverseGraphAndPopulateGraphicsObjects();
    }

    BasicGraphicsScene::~BasicGraphicsScene() = default;

    AbstractGraphModel const &BasicGraphicsScene::graphModel() const {
        return _graphModel;
    }

    AbstractGraphModel &BasicGraphicsScene::graphModel() {
        return _graphModel;
    }

    AbstractNodeGeometry &BasicGraphicsScene::nodeGeometry() {
        return *_nodeGeometry;
    }

    AbstractNodePainter &BasicGraphicsScene::nodePainter() {
        return *_nodePainter;
    }

    void BasicGraphicsScene::setNodePainter(std::unique_ptr<AbstractNodePainter> newPainter) {
        _nodePainter = std::move(newPainter);
    }

    QUndoStack &BasicGraphicsScene::undoStack() {
        return *_undoStack;
    }

    std::unique_ptr<ConnectionGraphicsObject> const &BasicGraphicsScene::makeDraftConnection(
            ConnectionId const incompleteConnectionId) {
        _draftConnection = std::make_unique<ConnectionGraphicsObject>(*this, incompleteConnectionId);

        _draftConnection->grabMouse();

        return _draftConnection;
    }

    void BasicGraphicsScene::resetDraftConnection() {
        _draftConnection.reset();
    }

    void BasicGraphicsScene::clearScene() {
        auto const &allNodeIds = graphModel().allNodeIds();

        for (auto nodeId: allNodeIds) {
            graphModel().deleteNode(nodeId);
        }
    }

    NodeGraphicsObject *BasicGraphicsScene::nodeGraphicsObject(NodeId nodeId) {
        NodeGraphicsObject *ngo = nullptr;
        auto it = _nodeGraphicsObjects.find(nodeId);
        if (it != _nodeGraphicsObjects.end()) {
            ngo = it->second.get();
        }

        return ngo;
    }

    ConnectionGraphicsObject *BasicGraphicsScene::connectionGraphicsObject(ConnectionId connectionId) {
        ConnectionGraphicsObject *cgo = nullptr;
        auto it = _connectionGraphicsObjects.find(connectionId);
        if (it != _connectionGraphicsObjects.end()) {
            cgo = it->second.get();
        }

        return cgo;
    }

    void BasicGraphicsScene::setOrientation(Qt::Orientation const orientation) {
        if (_orientation != orientation) {
            _orientation = orientation;

            switch (_orientation) {
                case Qt::Horizontal:
                    _nodeGeometry = std::make_unique<DefaultHorizontalNodeGeometry>(_graphModel);
                    break;

                case Qt::Vertical:
                    _nodeGeometry = std::make_unique<DefaultVerticalNodeGeometry>(_graphModel);
                    break;
            }
            onModelReset();
        }
    }

    void BasicGraphicsScene::toggleWidgetMode() {
        _orientation = Qt::Horizontal;
        _nodeGeometry = std::make_unique<WidgetHorizontalNodeGeometry>(_graphModel);
        onModelReset();
    }

    QMenu *BasicGraphicsScene::createSceneMenu(QPointF const scenePos) {
        Q_UNUSED(scenePos);
        return nullptr;
    }

    void BasicGraphicsScene::traverseGraphAndPopulateGraphicsObjects() {
        auto allNodeIds = _graphModel.allNodeIds();

        // First create all the nodes.
        for (NodeId const nodeId: allNodeIds) {
            _nodeGraphicsObjects[nodeId] = std::make_unique<NodeGraphicsObject>(*this, nodeId);
        }

        // Then for each node check output connections and insert them.
        for (NodeId const nodeId: allNodeIds) {
            unsigned int nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

            for (PortIndex index = 0; index < nOutPorts; ++index) {
                auto const &outConnectionIds = _graphModel.connections(nodeId, PortType::Out, index);

                for (auto cid: outConnectionIds) {
                    _connectionGraphicsObjects[cid] = std::make_unique<ConnectionGraphicsObject>(*this,
                                                                                                 cid);
                }
            }
        }
    }

    void BasicGraphicsScene::updateAttachedNodes(ConnectionId const connectionId,
                                                 PortType const portType) {
        auto node = nodeGraphicsObject(getNodeId(portType, connectionId));

        if (node) {
            node->update();
        }
    }

    void BasicGraphicsScene::onConnectionDeleted(ConnectionId const connectionId) {
        auto it = _connectionGraphicsObjects.find(connectionId);
        if (it != _connectionGraphicsObjects.end()) {
            _connectionGraphicsObjects.erase(it);
        }

        // TODO: do we need it?
        if (_draftConnection && _draftConnection->connectionId() == connectionId) {
            _draftConnection.reset();
        }

        updateAttachedNodes(connectionId, PortType::Out);
        updateAttachedNodes(connectionId, PortType::In);
    }

    void BasicGraphicsScene::onConnectionCreated(ConnectionId const connectionId) {
        _connectionGraphicsObjects[connectionId]
                = std::make_unique<ConnectionGraphicsObject>(*this, connectionId);

        updateAttachedNodes(connectionId, PortType::Out);
        updateAttachedNodes(connectionId, PortType::In);
    }

    void BasicGraphicsScene::onNodeDeleted(NodeId const nodeId) {
        auto it = _nodeGraphicsObjects.find(nodeId);
        if (it != _nodeGraphicsObjects.end()) {
            _nodeGraphicsObjects.erase(it);
        }
    }

    void BasicGraphicsScene::onNodeCreated(NodeId const nodeId) {
        _nodeGraphicsObjects[nodeId] = std::make_unique<NodeGraphicsObject>(*this, nodeId);
    }

    void BasicGraphicsScene::onNodePositionUpdated(NodeId const nodeId) {
        auto node = nodeGraphicsObject(nodeId);
        if (node) {
            node->setPos(_graphModel.nodeData(nodeId, NodeRole::Position).value<QPointF>());
            node->update();
            _nodeDrag = true;
        }
    }

    void BasicGraphicsScene::onNodeUpdated(NodeId const nodeId) {
        auto node = nodeGraphicsObject(nodeId);

        if (node) {
            node->setGeometryChanged();

            _nodeGeometry->recomputeSize(nodeId);

            node->update();
            node->moveConnections();
        }
    }

    void BasicGraphicsScene::onNodeClicked(NodeId const nodeId) {
        if (_nodeDrag)
                Q_EMIT nodeMoved(nodeId, _graphModel.nodeData(nodeId, NodeRole::Position).value<QPointF>());
        _nodeDrag = false;
    }

    void BasicGraphicsScene::onModelReset() {
        _connectionGraphicsObjects.clear();
        _nodeGraphicsObjects.clear();

        clear();

        traverseGraphAndPopulateGraphicsObjects();
    }

    bool BasicGraphicsScene::portVacant(NodeId nodeId, PortIndex const portIndex,
                          PortType const portType) {
        auto const connected = _graphModel.connections(nodeId, portType, portIndex);

        auto policy = _graphModel.portData(nodeId, portType, portIndex,
                                           PortRole::ConnectionPolicyRole)
                .value<ConnectionPolicy>();

        return connected.empty() || (policy == ConnectionPolicy::Many);
    };

    NodeDataType BasicGraphicsScene::getDataType(NodeId nodeId, PortIndex const portIndex,
                           PortType const portType) {
        return _graphModel.portData(nodeId, portType,
                                    portIndex,
                                    PortRole::DataType).value<NodeDataType>();
    };

    void BasicGraphicsScene::keyPressEvent(QKeyEvent *event) {
        QGraphicsScene::keyPressEvent(event);
        if (event->key() == Qt::Key_F) {
            // TODO: clean up big time
            auto sItems = selectedItems();
            if (sItems.size() == 2) {
                // Only fuse when two items are selected
                NodeId id1, id2;
                double pos1, pos2;
                if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(sItems[0])) {
                    id1 = n->nodeId();
                    pos1 = n->pos().x();
                } else {
                    return;
                }
                if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(sItems[1])) {
                    id2 = n->nodeId();
                    pos2 = n->pos().x();
                } else {
                    return;
                }
                // Sort them accordingly
                if(pos2 < pos1) {
                    std::swap(id1, id2);
                }
                if(_graphModel.nodeData<int>(id1, NodeRole::OutPortCount) == 0) {
                    std::swap(id1, id2);
                }


                // Find free port for node 1
                PortIndex p1 = 0;
                while (!portVacant(id1, p1, PortType::Out)) {
                    p1++;
                    if (getDataType(id1, p1, PortType::Out).id == InvalidData().type().id) {

                        std::swap(id1, id2); // Swap back and try again;
                        qDebug() << "No vacant ports";
                        p1 = 0;
                        break;
                    }
                }
                // Find free port for node 1
                while (!portVacant(id1, p1, PortType::Out)) {
                    p1++;
                    if (getDataType(id1, p1, PortType::Out).id == InvalidData().type().id) {
                        return; // No port available
                    }
                }
                NodeDataType p1Type = getDataType(id1, p1, PortType::Out);


                // Find free port for node 2
                // Also ensure the data types match
                PortIndex p2 = 0;
                while (!portVacant(id2, p2, PortType::In) &&
                       getDataType(id2, p2, PortType::In).id != p1Type.id) {
                    p2++;
                    if (getDataType(id2, p2, PortType::In).id == InvalidData().type().id) {
                        return; // No port available
                    }
                }

                ConnectionId connectionId = {.outNodeId = id1, .outPortIndex = p1, .inNodeId = id2, .inPortIndex = p1};

                auto getDataTypeFromPort = [&](PortType const portType) {
                    return _graphModel.portData(getNodeId(portType, connectionId), portType,
                                                getPortIndex(portType, connectionId),
                                                PortRole::DataType).value<NodeDataType>();
                };

                // Check if connection possible
                if (getDataTypeFromPort(PortType::Out).id == getDataTypeFromPort(PortType::In).id) {

                    NodeId const nodeId = getNodeId(PortType::In, connectionId);
                    PortIndex const portIndex = getPortIndex(PortType::In, connectionId);
                    auto const connections = _graphModel.connections(nodeId, PortType::In, portIndex);

                    _graphModel.addConnection(connectionId);
                }

            }
        }
    }


} // namespace QtNodes
