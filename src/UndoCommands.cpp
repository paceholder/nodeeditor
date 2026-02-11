#include "UndoCommands.hpp"

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "Definitions.hpp"
#include "GroupGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"

#include <unordered_set>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsObject>

namespace QtNodes {

static QJsonObject serializeSelectedItems(BasicGraphicsScene *scene)
{
    QJsonObject serializedScene;

    auto &graphModel = scene->graphModel();

    std::unordered_set<NodeId> selectedNodes;

    QJsonArray nodesJsonArray;
    QJsonArray groupsJsonArray;
    QJsonArray connJsonArray;

    auto appendNode = [&](NodeGraphicsObject *node) {
        if (!node)
            return;

        auto const inserted = selectedNodes.insert(node->nodeId());
        if (inserted.second) {
            nodesJsonArray.append(graphModel.saveNode(node->nodeId()));
        }
    };

    for (QGraphicsItem *item : scene->selectedItems()) {
        if (auto group = qgraphicsitem_cast<GroupGraphicsObject *>(item)) {
            for (auto *node : group->group().childNodes()) {
                appendNode(node);

                for (auto const &connectionId : graphModel.allConnectionIds(node->nodeId())) {
                    connJsonArray.append(toJson(connectionId));
                }
            }
        }
    }

    for (QGraphicsItem *item : scene->selectedItems()) {
        if (auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
            appendNode(ngo);

            for (auto const &connectionId : graphModel.allConnectionIds(ngo->nodeId())) {
                connJsonArray.append(toJson(connectionId));
            }
        }
    }

    for (QGraphicsItem *item : scene->selectedItems()) {
        if (auto groupGo = qgraphicsitem_cast<GroupGraphicsObject *>(item)) {
            auto &group = groupGo->group();

            QJsonObject groupJson;
            groupJson["id"] = static_cast<qint64>(group.id());
            groupJson["name"] = group.name();

            QJsonArray nodeIdsJson;
            for (NodeGraphicsObject *node : group.childNodes()) {
                nodeIdsJson.append(static_cast<qint64>(node->nodeId()));
            }

            groupJson["nodes"] = nodeIdsJson;
            groupsJsonArray.append(groupJson);
        }
    }

    for (QGraphicsItem *item : scene->selectedItems()) {
        if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject *>(item)) {
            auto const &cid = c->connectionId();

            if (selectedNodes.count(cid.outNodeId) > 0 && selectedNodes.count(cid.inNodeId) > 0) {
                connJsonArray.append(toJson(cid));
            }
        }
    }

    serializedScene["groups"] = groupsJsonArray;
    serializedScene["nodes"] = nodesJsonArray;
    serializedScene["connections"] = connJsonArray;

    return serializedScene;
}

static void insertSerializedItems(QJsonObject const &json, BasicGraphicsScene *scene)
{
    AbstractGraphModel &graphModel = scene->graphModel();

    QJsonArray const &nodesJsonArray = json["nodes"].toArray();

    for (QJsonValue node : nodesJsonArray) {
        QJsonObject obj = node.toObject();

        graphModel.loadNode(obj);

        auto id = obj["id"].toInt();
        scene->nodeGraphicsObject(id)->setZValue(1.0);
        scene->nodeGraphicsObject(id)->setSelected(true);
    }

    QJsonArray const &connJsonArray = json["connections"].toArray();

    for (QJsonValue connection : connJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId = fromJson(connJson);

        // Restore the connection
        graphModel.addConnection(connId);

        scene->connectionGraphicsObject(connId)->setSelected(true);
    }

    if (json.contains("groups")) {
        QJsonArray groupsJsonArray = json["groups"].toArray();

        for (const QJsonValue &groupValue : groupsJsonArray) {
            QJsonObject groupJson = groupValue.toObject();

            QString name = QString("Group %1").arg(NodeGroup::groupCount());
            QJsonArray nodeIdsJson = groupJson["nodes"].toArray();

            std::vector<NodeGraphicsObject *> groupNodes;

            for (const QJsonValue &idVal : nodeIdsJson) {
                NodeId nodeId = static_cast<NodeId>(idVal.toInt());
                if (auto *ngo = scene->nodeGraphicsObject(nodeId)) {
                    groupNodes.push_back(ngo);
                }
            }

            scene->createGroup(groupNodes, name);
        }
    }
}

static void deleteSerializedItems(QJsonObject &sceneJson, AbstractGraphModel &graphModel)
{
    QJsonArray connectionJsonArray = sceneJson["connections"].toArray();

    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId = fromJson(connJson);

        graphModel.deleteConnection(connId);
    }

    QJsonArray nodesJsonArray = sceneJson["nodes"].toArray();

    for (QJsonValueRef node : nodesJsonArray) {
        QJsonObject nodeJson = node.toObject();
        graphModel.deleteNode(nodeJson["id"].toInt());
    }
}

static QPointF computeAverageNodePosition(QJsonObject const &sceneJson)
{
    QPointF averagePos(0, 0);

    QJsonArray nodesJsonArray = sceneJson["nodes"].toArray();

    for (QJsonValueRef node : nodesJsonArray) {
        QJsonObject nodeJson = node.toObject();

        averagePos += QPointF(nodeJson["position"].toObject()["x"].toDouble(),
                              nodeJson["position"].toObject()["y"].toDouble());
    }

    averagePos /= static_cast<double>(nodesJsonArray.size());

    return averagePos;
}

//-------------------------------------

CreateCommand::CreateCommand(BasicGraphicsScene *scene,
                             QString const name,
                             QPointF const &mouseScenePos)
    : _scene(scene)
    , _sceneJson(QJsonObject())
{
    _nodeId = _scene->graphModel().addNode(name);
    if (_nodeId != InvalidNodeId) {
        _scene->graphModel().setNodeData(_nodeId, NodeRole::Position, mouseScenePos);
    } else {
        setObsolete(true);
    }
}

void CreateCommand::undo()
{
    QJsonArray nodesJsonArray;
    nodesJsonArray.append(_scene->graphModel().saveNode(_nodeId));
    _sceneJson["nodes"] = nodesJsonArray;

    _scene->graphModel().deleteNode(_nodeId);
}

void CreateCommand::redo()
{
    if (_sceneJson.empty() || _sceneJson["nodes"].toArray().empty())
        return;

    insertSerializedItems(_sceneJson, _scene);
}

//-------------------------------------

DeleteCommand::DeleteCommand(BasicGraphicsScene *scene)
    : _scene(scene)
{
    auto &graphModel = _scene->graphModel();

    QJsonArray connJsonArray;
    // Delete the selected connections first, ensuring that they won't be
    // automatically deleted when selected nodes are deleted (deleting a
    // node deletes some connections as well)
    for (QGraphicsItem *item : _scene->selectedItems()) {
        if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject *>(item)) {
            auto const &cid = c->connectionId();

            connJsonArray.append(toJson(cid));
        }
    }

    QJsonArray nodesJsonArray;
    // Delete the nodes; this will delete many of the connections.
    // Selected connections were already deleted prior to this loop,

    std::unordered_set<NodeId> processedNodes;

    auto appendNode = [&](NodeGraphicsObject *node) {
        if (!node)
            return;

        auto const inserted = processedNodes.insert(node->nodeId());
        if (!inserted.second)
            return;

        for (auto const &cid : graphModel.allConnectionIds(node->nodeId())) {
            connJsonArray.append(toJson(cid));
        }

        nodesJsonArray.append(graphModel.saveNode(node->nodeId()));
    };

    QJsonArray groupsJsonArray;

    for (QGraphicsItem *item : _scene->selectedItems()) {
        if (auto groupGo = qgraphicsitem_cast<GroupGraphicsObject *>(item)) {
            auto &groupData = groupGo->group();

            QJsonArray groupNodeIdsJsonArray;
            for (NodeGraphicsObject *node : groupData.childNodes()) {
                appendNode(node);
                groupNodeIdsJsonArray.append(static_cast<qint64>(node->nodeId()));
            }

            QJsonObject groupJson;
            groupJson["id"] = static_cast<qint64>(groupData.id());
            groupJson["name"] = groupData.name();
            groupJson["nodes"] = groupNodeIdsJsonArray;
            groupsJsonArray.append(groupJson);
        }
    }

    for (QGraphicsItem *item : _scene->selectedItems()) {
        if (auto group = qgraphicsitem_cast<GroupGraphicsObject *>(item)) {
            for (auto *node : group->group().childNodes()) {
                appendNode(node);
            }
        }
    }

    for (QGraphicsItem *item : _scene->selectedItems()) {
        if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
            appendNode(n);
        }
    }

    // If nothing is deleted, cancel this operation
    if (connJsonArray.isEmpty() && nodesJsonArray.isEmpty() && groupsJsonArray.isEmpty())
        setObsolete(true);

    _sceneJson["nodes"] = nodesJsonArray;
    _sceneJson["connections"] = connJsonArray;
    _sceneJson["groups"] = groupsJsonArray;
}

void DeleteCommand::undo()
{
    insertSerializedItems(_sceneJson, _scene);
}

void DeleteCommand::redo()
{
    deleteSerializedItems(_sceneJson, _scene->graphModel());
}

//-------------------------------------

void offsetNodeGroup(QJsonObject &sceneJson, QPointF const &diff)
{
    QJsonArray nodesJsonArray = sceneJson["nodes"].toArray();

    QJsonArray newNodesJsonArray;
    for (QJsonValueRef node : nodesJsonArray) {
        QJsonObject obj = node.toObject();

        QPointF oldPos(obj["position"].toObject()["x"].toDouble(),
                       obj["position"].toObject()["y"].toDouble());

        oldPos += diff;

        QJsonObject posJson;
        posJson["x"] = oldPos.x();
        posJson["y"] = oldPos.y();
        obj["position"] = posJson;

        newNodesJsonArray.append(obj);
    }

    sceneJson["nodes"] = newNodesJsonArray;
}

//-------------------------------------

CopyCommand::CopyCommand(BasicGraphicsScene *scene)
{
    QJsonObject sceneJson = serializeSelectedItems(scene);

    if (sceneJson.empty() || sceneJson["nodes"].toArray().empty()) {
        setObsolete(true);
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();

    QByteArray const data = QJsonDocument(sceneJson).toJson();

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("application/qt-nodes-graph", data);
    mimeData->setText(data);

    clipboard->setMimeData(mimeData);

    // Copy command does not have any effective redo/undo operations.
    // It copies the data to the clipboard and could be immediately removed
    // from the stack.
    setObsolete(true);
}

//-------------------------------------

PasteCommand::PasteCommand(BasicGraphicsScene *scene, QPointF const &mouseScenePos)
    : _scene(scene)
    , _mouseScenePos(mouseScenePos)
{
    _newSceneJson = takeSceneJsonFromClipboard();

    if (_newSceneJson.empty() || _newSceneJson["nodes"].toArray().empty()) {
        setObsolete(true);
        return;
    }

    _newSceneJson = makeNewNodeIdsInScene(_newSceneJson);

    QPointF averagePos = computeAverageNodePosition(_newSceneJson);

    offsetNodeGroup(_newSceneJson, _mouseScenePos - averagePos);
}

void PasteCommand::undo()
{
    deleteSerializedItems(_newSceneJson, _scene->graphModel());
}

void PasteCommand::redo()
{
    _scene->clearSelection();

    // Ignore if pasted in content does not generate nodes.
    try {
        insertSerializedItems(_newSceneJson, _scene);
    } catch (...) {
        // If the paste does not work, delete all selected nodes and connections
        // `deleteNode(...)` implicitly removed connections
        auto &graphModel = _scene->graphModel();

        QJsonArray nodesJsonArray;
        for (QGraphicsItem *item : _scene->selectedItems()) {
            if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
                graphModel.deleteNode(n->nodeId());
            }
        }

        setObsolete(true);
    }
}

QJsonObject PasteCommand::takeSceneJsonFromClipboard()
{
    QClipboard const *clipboard = QApplication::clipboard();
    QMimeData const *mimeData = clipboard->mimeData();

    QJsonDocument json;
    if (mimeData->hasFormat("application/qt-nodes-graph")) {
        json = QJsonDocument::fromJson(mimeData->data("application/qt-nodes-graph"));
    } else if (mimeData->hasText()) {
        json = QJsonDocument::fromJson(mimeData->text().toUtf8());
    }

    return json.object();
}

QJsonObject PasteCommand::makeNewNodeIdsInScene(QJsonObject const &sceneJson)
{
    AbstractGraphModel &graphModel = _scene->graphModel();

    std::unordered_map<NodeId, NodeId> mapNodeIds;

    QJsonArray nodesJsonArray = sceneJson["nodes"].toArray();

    QJsonArray newNodesJsonArray;
    for (QJsonValueRef node : nodesJsonArray) {
        QJsonObject nodeJson = node.toObject();

        NodeId oldNodeId = nodeJson["id"].toInt();

        NodeId newNodeId = graphModel.newNodeId();

        mapNodeIds[oldNodeId] = newNodeId;

        // Replace NodeId in json
        nodeJson["id"] = static_cast<qint64>(newNodeId);

        newNodesJsonArray.append(nodeJson);
    }

    QJsonArray connectionJsonArray = sceneJson["connections"].toArray();

    QJsonArray newConnJsonArray;
    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId = fromJson(connJson);

        ConnectionId newConnId{mapNodeIds[connId.outNodeId],
                               connId.outPortIndex,
                               mapNodeIds[connId.inNodeId],
                               connId.inPortIndex};

        newConnJsonArray.append(toJson(newConnId));
    }

    QJsonObject newSceneJson;

    newSceneJson["nodes"] = newNodesJsonArray;
    newSceneJson["connections"] = newConnJsonArray;

    if (sceneJson.contains("groups")) {
        QJsonArray groupsJsonArray = sceneJson["groups"].toArray();
        QJsonArray newGroupsJsonArray;

        for (const QJsonValue &groupVal : groupsJsonArray) {
            QJsonObject groupJson = groupVal.toObject();
            QJsonArray nodeIdsJson = groupJson["nodes"].toArray();

            QJsonArray newNodeIdsJson;
            for (const QJsonValue &idVal : nodeIdsJson) {
                NodeId oldId = static_cast<NodeId>(idVal.toInt());
                NodeId newId = mapNodeIds[oldId];
                newNodeIdsJson.append(static_cast<qint64>(newId));
            }

            groupJson["nodes"] = newNodeIdsJson;
            newGroupsJsonArray.append(groupJson);
        }

        newSceneJson["groups"] = newGroupsJsonArray;
    }

    return newSceneJson;
}

//-------------------------------------

DisconnectCommand::DisconnectCommand(BasicGraphicsScene *scene, ConnectionId const connId)
    : _scene(scene)
    , _connId(connId)
{
    //
}

void DisconnectCommand::undo()
{
    _scene->graphModel().addConnection(_connId);
}

void DisconnectCommand::redo()
{
    _scene->graphModel().deleteConnection(_connId);
}

//------

ConnectCommand::ConnectCommand(BasicGraphicsScene *scene, ConnectionId const connId)
    : _scene(scene)
    , _connId(connId)
{
    //
}

void ConnectCommand::undo()
{
    _scene->graphModel().deleteConnection(_connId);
}

void ConnectCommand::redo()
{
    _scene->graphModel().addConnection(_connId);
}

//------

MoveNodeCommand::MoveNodeCommand(BasicGraphicsScene *scene, QPointF const &diff)
    : _scene(scene)
    , _diff(diff)
{
    _selectedNodes.clear();
    for (QGraphicsItem *item : _scene->selectedItems()) {
        if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
            _selectedNodes.insert(n->nodeId());
        }
    }
}

void MoveNodeCommand::undo()
{
    for (auto nodeId : _selectedNodes) {
        auto oldPos = _scene->graphModel().nodeData(nodeId, NodeRole::Position).value<QPointF>();

        oldPos -= _diff;

        _scene->graphModel().setNodeData(nodeId, NodeRole::Position, oldPos);
    }
}

void MoveNodeCommand::redo()
{
    for (auto nodeId : _selectedNodes) {
        auto oldPos = _scene->graphModel().nodeData(nodeId, NodeRole::Position).value<QPointF>();

        oldPos += _diff;

        _scene->graphModel().setNodeData(nodeId, NodeRole::Position, oldPos);
    }
}

int MoveNodeCommand::id() const
{
    return static_cast<int>(typeid(MoveNodeCommand).hash_code());
}

bool MoveNodeCommand::mergeWith(QUndoCommand const *c)
{
    auto mc = static_cast<MoveNodeCommand const *>(c);

    if (_selectedNodes == mc->_selectedNodes) {
        _diff += mc->_diff;
        return true;
    }
    return false;
}

} // namespace QtNodes
