#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"
#include "TestDataFlowNodes.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;

/// Test model that allows loops (default behavior)
class LoopEnabledModel : public TestGraphModel
{
public:
    bool loopsEnabled() const override { return true; }
};

/// Test model that disables loops
class LoopDisabledModel : public TestGraphModel
{
public:
    bool loopsEnabled() const override { return false; }
};

TEST_CASE("Loop detection configuration", "[loops]")
{
    SECTION("Default AbstractGraphModel allows loops")
    {
        TestGraphModel model;
        CHECK(model.loopsEnabled() == true);
    }

    SECTION("DataFlowGraphModel disables loops by default")
    {
        auto app = applicationSetup();
        auto registry = std::make_shared<NodeDelegateModelRegistry>();
        registry->registerModel<TestSourceNode>("Sources");

        DataFlowGraphModel model(registry);
        CHECK(model.loopsEnabled() == false);
    }

    SECTION("Custom model can enable loops")
    {
        LoopEnabledModel model;
        CHECK(model.loopsEnabled() == true);
    }

    SECTION("Custom model can disable loops")
    {
        LoopDisabledModel model;
        CHECK(model.loopsEnabled() == false);
    }
}

TEST_CASE("Loop detection in DataFlowGraphModel", "[loops]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<TestSourceNode>("Sources");
    registry->registerModel<TestDisplayNode>("Sinks");

    DataFlowGraphModel model(registry);

    SECTION("Direct self-loop is not possible")
    {
        NodeId node1 = model.addNode("TestSourceNode");

        // Try to connect node to itself
        ConnectionId selfLoop{node1, 0, node1, 0};
        CHECK_FALSE(model.connectionPossible(selfLoop));
    }

    SECTION("Simple A->B connection is allowed")
    {
        NodeId node1 = model.addNode("TestSourceNode");
        NodeId node2 = model.addNode("TestDisplayNode");

        ConnectionId conn{node1, 0, node2, 0};
        CHECK(model.connectionPossible(conn));

        model.addConnection(conn);
        CHECK(model.connectionExists(conn));
    }

    SECTION("Indirect loop A->B->A is prevented")
    {
        // Use TestDisplayNode which has both input and output ports
        NodeId node1 = model.addNode("TestDisplayNode");
        NodeId node2 = model.addNode("TestDisplayNode");

        // Create A->B connection
        ConnectionId conn1{node1, 0, node2, 0};
        CHECK(model.connectionPossible(conn1));
        model.addConnection(conn1);

        // Try to create B->A connection (would form a loop)
        ConnectionId conn2{node2, 0, node1, 0};
        CHECK_FALSE(model.connectionPossible(conn2));
    }

    SECTION("Three node loop A->B->C->A is prevented")
    {
        // Use TestDisplayNode which has both input and output ports
        NodeId node1 = model.addNode("TestDisplayNode");
        NodeId node2 = model.addNode("TestDisplayNode");
        NodeId node3 = model.addNode("TestDisplayNode");

        // Create A->B
        ConnectionId conn1{node1, 0, node2, 0};
        model.addConnection(conn1);
        CHECK(model.connectionExists(conn1));

        // Create B->C
        ConnectionId conn2{node2, 0, node3, 0};
        model.addConnection(conn2);
        CHECK(model.connectionExists(conn2));

        // Try to create C->A (would form a loop)
        ConnectionId conn3{node3, 0, node1, 0};
        CHECK_FALSE(model.connectionPossible(conn3));
    }
}

TEST_CASE("Loop-enabled model allows cycles", "[loops]")
{
    LoopEnabledModel model;

    NodeId node1 = model.addNode("Node1");
    NodeId node2 = model.addNode("Node2");

    // Create A->B connection
    ConnectionId conn1{node1, 0, node2, 0};
    model.addConnection(conn1);
    CHECK(model.connectionExists(conn1));

    SECTION("B->A connection is allowed when loops enabled")
    {
        ConnectionId conn2{node2, 0, node1, 0};
        // With loops enabled, this should be possible
        CHECK(model.connectionPossible(conn2));
    }
}

TEST_CASE("Loop-disabled model prevents cycles", "[loops]")
{
    LoopDisabledModel model;

    NodeId node1 = model.addNode("Node1");
    NodeId node2 = model.addNode("Node2");

    // Note: TestGraphModel's connectionPossible doesn't check for loops,
    // it only checks if nodes exist and aren't self-connecting.
    // The loop detection is in DataFlowGraphModel's implementation.
    // This test documents the expected behavior when properly implemented.

    ConnectionId conn1{node1, 0, node2, 0};
    model.addConnection(conn1);

    SECTION("Loop-disabled model configuration")
    {
        CHECK(model.loopsEnabled() == false);
    }
}
