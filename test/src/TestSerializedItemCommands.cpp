#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/Definitions>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/GroupGraphicsObject.hpp>
#include <QtNodes/internal/NodeGroup.hpp>
#include <QtNodes/internal/UndoCommands.hpp>

#include <catch2/catch.hpp>

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QMimeData>
#include <QUndoStack>

#include <algorithm>
#include <vector>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionId;
using QtNodes::CopyCommand;
using QtNodes::CreateCommand;
using QtNodes::DeleteCommand;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PasteCommand;

namespace {

/// Records the ids that reach deleteNode(). The first real node has the id 5.
class RecordingGraphModel : public TestGraphModel
{
public:
    RecordingGraphModel()
    {
        // Burn the ids 1 to 4.
        for (int i = 0; i < 4; ++i) {
            NodeId const id = addNode("Unused");
            TestGraphModel::deleteNode(id);
        }
    }

    bool deleteNode(NodeId const nodeId) override
    {
        deletedIds.push_back(nodeId);
        return TestGraphModel::deleteNode(nodeId);
    }

    std::vector<NodeId> deletedIds;
};

NodeId addNodeAt(RecordingGraphModel &model, QPointF const &pos)
{
    NodeId const id = model.addNode("TestNode");
    model.setNodeData(id, NodeRole::Position, pos);
    return id;
}

void selectOnly(BasicGraphicsScene &scene, std::vector<NodeId> const &ids)
{
    scene.clearSelection();
    for (NodeId id : ids) {
        auto *ngo = scene.nodeGraphicsObject(id);
        REQUIRE(ngo != nullptr);
        ngo->setSelected(true);
    }
}

} // namespace

TEST_CASE("Test nodes get ids that do not start at zero", "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));

    CHECK(a == 5);
    CHECK(b == 6);
}

TEST_CASE("DeleteCommand deletes the selected node and not another one", "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(200, 0));
    model.deletedIds.clear();

    // Select the middle node.
    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));

    CHECK(model.deletedIds == std::vector<NodeId>{b});
    CHECK(model.nodeExists(a));
    CHECK_FALSE(model.nodeExists(b));
    CHECK(model.nodeExists(c));
}

TEST_CASE("DeleteCommand passes the id of each selected node to deleteNode",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(200, 0));
    NodeId const d = addNodeAt(model, QPointF(300, 0));
    model.deletedIds.clear();

    selectOnly(scene, {a, c});
    scene.undoStack().push(new DeleteCommand(&scene));

    std::sort(model.deletedIds.begin(), model.deletedIds.end());
    CHECK(model.deletedIds == std::vector<NodeId>{a, c});
    CHECK_FALSE(model.nodeExists(a));
    CHECK(model.nodeExists(b));
    CHECK_FALSE(model.nodeExists(c));
    CHECK(model.nodeExists(d));
}

TEST_CASE("Undo of a delete restores the node and its position, redo deletes it again",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(120, 80));
    NodeId const c = addNodeAt(model, QPointF(200, 0));

    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));
    REQUIRE_FALSE(model.nodeExists(b));

    scene.undoStack().undo();

    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(b));
    CHECK(model.nodeExists(c));
    CHECK(model.nodeData(b, NodeRole::Position).toPointF() == QPointF(120, 80));
    CHECK(scene.nodeGraphicsObject(b) != nullptr);

    model.deletedIds.clear();
    scene.undoStack().redo();

    CHECK(model.deletedIds == std::vector<NodeId>{b});
    CHECK_FALSE(model.nodeExists(b));
    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(c));
    CHECK(scene.nodeGraphicsObject(b) == nullptr);
}

TEST_CASE("Undo of a delete restores several nodes", "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(10, 20));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(30, 40));

    selectOnly(scene, {a, c});
    scene.undoStack().push(new DeleteCommand(&scene));
    REQUIRE(model.allNodeIds().size() == 1);

    scene.undoStack().undo();

    CHECK(model.allNodeIds().size() == 3);
    CHECK(model.nodeData(a, NodeRole::Position).toPointF() == QPointF(10, 20));
    CHECK(model.nodeData(b, NodeRole::Position).toPointF() == QPointF(100, 0));
    CHECK(model.nodeData(c, NodeRole::Position).toPointF() == QPointF(30, 40));
    CHECK(scene.nodeGraphicsObject(a) != nullptr);
    CHECK(scene.nodeGraphicsObject(c) != nullptr);
}

TEST_CASE("Deleting a node removes its connections and undo restores them",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(200, 0));

    ConnectionId const ab{a, 0, b, 0};
    ConnectionId const bc{b, 0, c, 0};
    ConnectionId const ac{a, 0, c, 0};
    model.addConnection(ab);
    model.addConnection(bc);
    model.addConnection(ac);

    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));

    CHECK_FALSE(model.connectionExists(ab));
    CHECK_FALSE(model.connectionExists(bc));
    CHECK(model.connectionExists(ac));
    CHECK(scene.connectionGraphicsObject(ac) != nullptr);

    scene.undoStack().undo();

    CHECK(model.connectionExists(ab));
    CHECK(model.connectionExists(bc));
    CHECK(model.connectionExists(ac));
    CHECK(scene.connectionGraphicsObject(ab) != nullptr);
    CHECK(scene.connectionGraphicsObject(bc) != nullptr);

    scene.undoStack().redo();

    CHECK_FALSE(model.connectionExists(ab));
    CHECK_FALSE(model.connectionExists(bc));
    CHECK(model.connectionExists(ac));
}

TEST_CASE("CreateCommand adds a node, undo removes it and redo restores it",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const other = addNodeAt(model, QPointF(0, 0));
    model.deletedIds.clear();

    scene.undoStack().push(new CreateCommand(&scene, "TestNode", QPointF(50, 60)));

    REQUIRE(model.allNodeIds().size() == 2);
    NodeId created = other;
    for (NodeId id : model.allNodeIds()) {
        if (id != other)
            created = id;
    }
    REQUIRE(created != other);
    CHECK(model.nodeData(created, NodeRole::Position).toPointF() == QPointF(50, 60));

    scene.undoStack().undo();

    CHECK(model.deletedIds == std::vector<NodeId>{created});
    CHECK_FALSE(model.nodeExists(created));
    CHECK(model.nodeExists(other));

    scene.undoStack().redo();

    CHECK(model.nodeExists(created));
    CHECK(model.nodeData(created, NodeRole::Position).toPointF() == QPointF(50, 60));
    CHECK(scene.nodeGraphicsObject(created) != nullptr);
}

TEST_CASE("Paste creates new node ids and keeps the connections between pasted nodes",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(200, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});
    model.addConnection(ConnectionId{b, 0, c, 0});

    // Copy only a and b. The connection b -> c leaves the selection.
    selectOnly(scene, {a, b});
    scene.undoStack().push(new CopyCommand(&scene));
    scene.undoStack().push(new PasteCommand(&scene, QPointF(500, 500)));

    REQUIRE(model.allNodeIds().size() == 5);

    std::vector<NodeId> added;
    for (NodeId id : model.allNodeIds()) {
        if (id != a && id != b && id != c)
            added.push_back(id);
    }
    std::sort(added.begin(), added.end());
    REQUIRE(added.size() == 2);
    CHECK(added[0] > c);
    CHECK(added[1] > c);

    // The pasted nodes are connected like the originals.
    // The order of the new ids follows the selection order, so accept both directions.
    CHECK((model.connectionExists(ConnectionId{added[0], 0, added[1], 0})
           || model.connectionExists(ConnectionId{added[1], 0, added[0], 0})));
    CHECK(model.allConnectionIds(added[0]).size() == 1);
    CHECK(model.allConnectionIds(added[1]).size() == 1);

    // The original nodes and connections are untouched.
    CHECK(model.connectionExists(ConnectionId{a, 0, b, 0}));
    CHECK(model.connectionExists(ConnectionId{b, 0, c, 0}));

    // Pasted nodes are centered on the paste position.
    QPointF const p0 = model.nodeData(added[0], NodeRole::Position).toPointF();
    QPointF const p1 = model.nodeData(added[1], NodeRole::Position).toPointF();
    CHECK((p0 + p1) / 2.0 == QPointF(500, 500));
}

TEST_CASE("Undo of a paste deletes the pasted nodes by their new ids", "[undo][serialized]")
{
    auto app = applicationSetup();

    RecordingGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});

    selectOnly(scene, {a, b});
    scene.undoStack().push(new CopyCommand(&scene));
    scene.undoStack().push(new PasteCommand(&scene, QPointF(300, 300)));
    REQUIRE(model.allNodeIds().size() == 4);

    model.deletedIds.clear();
    scene.undoStack().undo();

    CHECK(model.allNodeIds().size() == 2);
    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(b));
    CHECK(model.connectionExists(ConnectionId{a, 0, b, 0}));
    REQUIRE(model.deletedIds.size() == 2);
    for (NodeId id : model.deletedIds) {
        CHECK(id != a);
        CHECK(id != b);
    }

    scene.undoStack().redo();
    CHECK(model.allNodeIds().size() == 4);
}

namespace {

/// Uses the default saveNode() and loadNode() of AbstractGraphModel.
/// The default saveNode() returns an empty object. The default loadNode() does nothing.
class DefaultSaveGraphModel : public RecordingGraphModel
{
public:
    QJsonObject saveNode(NodeId const nodeId) const override
    {
        return AbstractGraphModel::saveNode(nodeId);
    }

    void loadNode(QJsonObject const &nodeJson) override { AbstractGraphModel::loadNode(nodeJson); }
};

} // namespace

TEST_CASE("DeleteCommand deletes the selected node when the model keeps the default saveNode",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    DefaultSaveGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    model.deletedIds.clear();

    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));

    CHECK(model.deletedIds == std::vector<NodeId>{b});
    CHECK(model.nodeExists(a));
    CHECK_FALSE(model.nodeExists(b));
}

TEST_CASE("Undo and redo of a delete do not crash when the model keeps the default saveNode",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    DefaultSaveGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    NodeId const c = addNodeAt(model, QPointF(200, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});

    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));
    REQUIRE_FALSE(model.nodeExists(b));

    // The default loadNode() cannot restore the node.
    scene.undoStack().undo();
    CHECK_FALSE(model.nodeExists(b));
    CHECK(scene.nodeGraphicsObject(b) == nullptr);
    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(c));

    // Redo has nothing left to delete. The other nodes stay.
    scene.undoStack().redo();
    CHECK_FALSE(model.nodeExists(b));
    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(c));
}

TEST_CASE("Paste does not crash when the model keeps the default saveNode",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    DefaultSaveGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});

    selectOnly(scene, {a, b});
    scene.undoStack().push(new CopyCommand(&scene));
    scene.undoStack().push(new PasteCommand(&scene, QPointF(300, 300)));

    // The default loadNode() creates no nodes. The original nodes stay.
    CHECK(model.allNodeIds().size() == 2);
    CHECK(model.nodeExists(a));
    CHECK(model.nodeExists(b));
    CHECK(model.connectionExists(ConnectionId{a, 0, b, 0}));
}

namespace {

/// Node ids start above the range of a signed 32-bit integer.
class BigIdGraphModel : public RecordingGraphModel
{
public:
    NodeId newNodeId() override { return _nextBigId++; }

    static constexpr NodeId firstId = 3000000000u;

private:
    NodeId _nextBigId = firstId;
};

} // namespace

TEST_CASE("Delete and undo work for node ids above the signed 32-bit range",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    BigIdGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 50));
    REQUIRE(a == BigIdGraphModel::firstId);
    REQUIRE(b == BigIdGraphModel::firstId + 1);

    ConnectionId const ab{a, 0, b, 0};
    model.addConnection(ab);
    model.deletedIds.clear();

    selectOnly(scene, {b});
    scene.undoStack().push(new DeleteCommand(&scene));

    CHECK(model.deletedIds == std::vector<NodeId>{b});
    CHECK(model.nodeExists(a));
    CHECK_FALSE(model.nodeExists(b));

    scene.undoStack().undo();

    CHECK(model.nodeExists(b));
    CHECK(model.nodeData(b, NodeRole::Position).toPointF() == QPointF(100, 50));
    CHECK(model.connectionExists(ab));
    CHECK(scene.nodeGraphicsObject(b) != nullptr);

    model.deletedIds.clear();
    scene.undoStack().redo();

    CHECK(model.deletedIds == std::vector<NodeId>{b});
    CHECK_FALSE(model.nodeExists(b));
}

TEST_CASE("Paste keeps connections for node ids above the signed 32-bit range",
          "[undo][serialized]")
{
    auto app = applicationSetup();

    BigIdGraphModel model;
    BasicGraphicsScene scene(model);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});

    selectOnly(scene, {a, b});
    scene.undoStack().push(new CopyCommand(&scene));
    scene.undoStack().push(new PasteCommand(&scene, QPointF(300, 300)));

    REQUIRE(model.allNodeIds().size() == 4);
    for (NodeId id : model.allNodeIds())
        CHECK(id >= BigIdGraphModel::firstId);

    model.deletedIds.clear();
    scene.undoStack().undo();

    CHECK(model.allNodeIds().size() == 2);
    REQUIRE(model.deletedIds.size() == 2);
    for (NodeId id : model.deletedIds)
        CHECK(id >= BigIdGraphModel::firstId);
}

TEST_CASE("Loading serialized items restores a group and a connection for ids above the signed "
          "32-bit range",
          "[undo][serialized][group]")
{
    auto app = applicationSetup();

    BigIdGraphModel model;
    BasicGraphicsScene scene(model);
    scene.setGroupingEnabled(true);

    NodeId const a = addNodeAt(model, QPointF(0, 0));
    NodeId const b = addNodeAt(model, QPointF(100, 0));
    model.addConnection(ConnectionId{a, 0, b, 0});

    selectOnly(scene, {a, b});
    auto const group = scene.createGroupFromSelection("Big").lock();
    REQUIRE(group != nullptr);

    // Nodes in a group are locked. Select the group to copy it with its nodes.
    scene.clearSelection();
    group->groupGraphicsObject().setSelected(true);
    scene.undoStack().push(new CopyCommand(&scene));

    QMimeData const *mimeData = QApplication::clipboard()->mimeData();
    REQUIRE(mimeData != nullptr);
    QByteArray const data = mimeData->data("application/qt-nodes-graph");
    REQUIRE_FALSE(data.isEmpty());

    BigIdGraphModel otherModel;
    BasicGraphicsScene otherScene(otherModel);
    otherScene.setGroupingEnabled(true);
    otherScene.loadFromMemory(data);

    CHECK(otherModel.allNodeIds().size() == 2);
    REQUIRE(otherScene.groups().size() == 1);
    CHECK(otherScene.groups().begin()->second->childNodes().size() == 2);

    NodeId const first = *otherModel.allNodeIds().begin();
    CHECK(otherModel.allConnectionIds(first).size() == 1);
}
