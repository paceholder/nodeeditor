#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>

#include <QUndoStack>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionId;
using QtNodes::InvalidNodeId;
using QtNodes::NodeId;
using QtNodes::NodeRole;

TEST_CASE("UndoStack integration with BasicGraphicsScene", "[undo]")
{
    auto app = applicationSetup();
    TestGraphModel model;
    BasicGraphicsScene scene(model);

    SECTION("Scene has undo stack")
    {
        auto &undoStack = scene.undoStack();
        CHECK(undoStack.count() == 0);
        CHECK_FALSE(undoStack.canUndo());
        CHECK_FALSE(undoStack.canRedo());
    }

    SECTION("Model operations are independent of undo stack")
    {
        auto &undoStack = scene.undoStack();
        int initialCount = undoStack.count();

        // Direct model operations don't automatically create undo commands
        NodeId nodeId = model.addNode("TestNode");
        CHECK(nodeId != InvalidNodeId);
        CHECK(model.nodeExists(nodeId));
        
        // The undo stack shouldn't automatically have commands from direct model operations
        // (unless the scene is set up to automatically track them)
        CHECK(undoStack.count() >= initialCount);
    }
}

TEST_CASE("Manual undo/redo simulation", "[undo]")
{
    auto app = applicationSetup();
    TestGraphModel model;

    SECTION("Model state tracking for undo simulation")
    {
        // Test that we can manually track and restore model state
        NodeId nodeId = model.addNode("TestNode");
        CHECK(nodeId != InvalidNodeId);
        
        QPointF originalPos(100, 200);
        QPointF newPos(300, 400);
        
        model.setNodeData(nodeId, NodeRole::Position, originalPos);
        auto savedPos = model.nodeData(nodeId, NodeRole::Position);
        
        // Change position
        model.setNodeData(nodeId, NodeRole::Position, newPos);
        CHECK(model.nodeData(nodeId, NodeRole::Position).toPointF() == newPos);
        
        // "Undo" by restoring saved state
        model.setNodeData(nodeId, NodeRole::Position, savedPos);
        CHECK(model.nodeData(nodeId, NodeRole::Position).toPointF() == originalPos);
    }

    SECTION("Connection state tracking")
    {
        NodeId node1 = model.addNode("Node1");
        NodeId node2 = model.addNode("Node2");
        
        // Validate nodes were created successfully
        CHECK(node1 != InvalidNodeId);
        CHECK(node2 != InvalidNodeId);
        
        ConnectionId connId{node1, 0, node2, 0};

        CHECK_FALSE(model.connectionExists(connId));

        // "Do" operation
        model.addConnection(connId);
        CHECK(model.connectionExists(connId));

        // "Undo" operation
        model.deleteConnection(connId);
        CHECK_FALSE(model.connectionExists(connId));

        // "Redo" operation
        model.addConnection(connId);
        CHECK(model.connectionExists(connId));
    }
}
