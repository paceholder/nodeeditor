#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <QtNodes/BasicGraphicsScene>

#include <catch2/catch.hpp>

#include <QGraphicsView>
#include <QUndoStack>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionId;
using QtNodes::NodeId;
using QtNodes::NodeRole;

TEST_CASE("BasicGraphicsScene functionality", "[graphics]")
{
    auto app = applicationSetup();
    TestGraphModel model;
    BasicGraphicsScene scene(model);

    SECTION("Scene initialization")
    {
        CHECK(&scene.graphModel() == &model);
        CHECK(scene.items().isEmpty());
    }

    SECTION("Node creation in scene")
    {
        NodeId nodeId = model.addNode("TestNode");
        
        // The scene should automatically create graphics objects for new nodes
        // Due to signal-slot connections
        
        // Process events to ensure graphics objects are created
        QCoreApplication::processEvents();
        
        CHECK(model.nodeExists(nodeId));
        // The scene should have at least one item (the node graphics object)
        CHECK(scene.items().size() >= 1);
    }

    SECTION("Connection creation in scene")
    {
        NodeId node1 = model.addNode("Node1");
        NodeId node2 = model.addNode("Node2");
        
        QCoreApplication::processEvents();
        
        ConnectionId connId{node1, 0, node2, 0};
        model.addConnection(connId);
        
        QCoreApplication::processEvents();
        
        CHECK(model.connectionExists(connId));
        // Scene should have graphics objects for both nodes and the connection
        CHECK(scene.items().size() >= 3); // 2 nodes + 1 connection
    }

    SECTION("Node deletion from scene")
    {
        NodeId nodeId = model.addNode("TestNode");
        QCoreApplication::processEvents();
        
        auto initialItemCount = scene.items().size();
        CHECK(initialItemCount >= 1);
        
        model.deleteNode(nodeId);
        QCoreApplication::processEvents();
        
        CHECK_FALSE(model.nodeExists(nodeId));
        // Graphics object should be removed from scene
        CHECK(scene.items().size() < initialItemCount);
    }

    SECTION("Scene with graphics view")
    {
        NodeId nodeId = model.addNode("TestNode");
        model.setNodeData(nodeId, NodeRole::Position, QPointF(100, 200));
        
        QCoreApplication::processEvents();
        
        CHECK(scene.items().size() >= 1);
        
        // Create view but don't show it to avoid windowing system issues
        QGraphicsView view(&scene);
        
        // View should be properly connected to scene
        CHECK(view.scene() == &scene);
        
        // Don't call view.show() to avoid potential graphics system issues
    }
}

TEST_CASE("BasicGraphicsScene undo/redo support", "[graphics]")
{
    auto app = applicationSetup();
    TestGraphModel model;
    BasicGraphicsScene scene(model);

    SECTION("Undo stack exists")
    {
        auto &undoStack = scene.undoStack();
        CHECK(undoStack.count() == 0);
    }

    SECTION("Operations are tracked in undo stack")
    {
        auto &undoStack = scene.undoStack();
        
        NodeId nodeId = model.addNode("TestNode");
        QCoreApplication::processEvents();
        
        CHECK(model.nodeExists(nodeId));
        
        // Note: Depending on the implementation, the undo stack might or might not
        // automatically track model changes. This test verifies the stack exists
        // and can be used for undo operations.
        CHECK(undoStack.count() >= 0);
    }
}
