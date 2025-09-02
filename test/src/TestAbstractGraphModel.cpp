#include "TestGraphModel.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/Definitions>
#include <QtCore/QPointF>

using QtNodes::ConnectionId;
using QtNodes::InvalidNodeId;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortType;

TEST_CASE("AbstractGraphModel basic functionality", "[core]")
{
    TestGraphModel model;

    SECTION("Node creation and management")
    {
        CHECK(model.allNodeIds().empty());

        NodeId nodeId = model.addNode("TestNode");
        CHECK(nodeId != InvalidNodeId);
        CHECK(model.nodeExists(nodeId));
        CHECK(model.allNodeIds().size() == 1);
        CHECK(model.allNodeIds().count(nodeId) == 1);

        // Test node data
        CHECK(model.nodeData(nodeId, NodeRole::Type).toString() == "TestNode");
        CHECK(model.nodeData(nodeId, NodeRole::Caption).toString() == QString("Node %1").arg(nodeId));

        // Test setting node data
        bool result = model.setNodeData(nodeId, NodeRole::Position, QPointF(100, 200));
        CHECK(result);
        CHECK(model.nodeData<QPointF>(nodeId, NodeRole::Position) == QPointF(100, 200));
    }

    SECTION("Multiple nodes")
    {
        NodeId node1 = model.addNode("Node1");
        NodeId node2 = model.addNode("Node2");
        NodeId node3 = model.addNode("Node3");

        // Validate all nodes were created successfully
        CHECK(node1 != InvalidNodeId);
        CHECK(node2 != InvalidNodeId);
        CHECK(node3 != InvalidNodeId);

        CHECK(model.allNodeIds().size() == 3);
        CHECK(node1 != node2);
        CHECK(node2 != node3);
        CHECK(node1 != node3);

        CHECK(model.nodeExists(node1));
        CHECK(model.nodeExists(node2));
        CHECK(model.nodeExists(node3));
    }

    SECTION("Node deletion")
    {
        NodeId nodeId = model.addNode("TestNode");
        CHECK(nodeId != InvalidNodeId);
        CHECK(model.nodeExists(nodeId));

        bool result = model.deleteNode(nodeId);
        CHECK(result);
        CHECK_FALSE(model.nodeExists(nodeId));
        CHECK(model.allNodeIds().empty());

        // Try to delete non-existent node
        result = model.deleteNode(nodeId);
        CHECK_FALSE(result);
    }
}

TEST_CASE("Connection management", "[core]")
{
    TestGraphModel model;

    NodeId node1 = model.addNode("Node1");
    NodeId node2 = model.addNode("Node2");

    ConnectionId connId{node1, 0, node2, 0};

    SECTION("Connection creation")
    {
        CHECK_FALSE(model.connectionExists(connId));
        CHECK(model.connectionPossible(connId));

        model.addConnection(connId);
        CHECK(model.connectionExists(connId));

        // Check connections are properly tracked
        auto node1Connections = model.allConnectionIds(node1);
        auto node2Connections = model.allConnectionIds(node2);

        CHECK(node1Connections.size() == 1);
        CHECK(node2Connections.size() == 1);
        CHECK(node1Connections.count(connId) == 1);
        CHECK(node2Connections.count(connId) == 1);
    }

    SECTION("Connection validation")
    {
        // Self-connection should not be possible
        ConnectionId selfConn{node1, 0, node1, 0};
        CHECK_FALSE(model.connectionPossible(selfConn));

        // Connection to non-existent node should not be possible
        ConnectionId invalidConn{node1, 0, 999, 0};
        CHECK_FALSE(model.connectionPossible(invalidConn));
    }

    SECTION("Connection deletion")
    {
        model.addConnection(connId);
        CHECK(model.connectionExists(connId));

        bool result = model.deleteConnection(connId);
        CHECK(result);
        CHECK_FALSE(model.connectionExists(connId));

        // Try to delete non-existent connection
        result = model.deleteConnection(connId);
        CHECK_FALSE(result);
    }

    SECTION("Connections by port")
    {
        model.addConnection(connId);

        auto outConnections = model.connections(node1, PortType::Out, 0);
        auto inConnections = model.connections(node2, PortType::In, 0);

        CHECK(outConnections.size() == 1);
        CHECK(inConnections.size() == 1);
        CHECK(outConnections.count(connId) == 1);
        CHECK(inConnections.count(connId) == 1);

        // Check that wrong port type returns empty
        auto wrongOut = model.connections(node1, PortType::In, 0);
        auto wrongIn = model.connections(node2, PortType::Out, 0);
        CHECK(wrongOut.empty());
        CHECK(wrongIn.empty());
    }
}

TEST_CASE("Node deletion with connections", "[core]")
{
    TestGraphModel model;

    NodeId node1 = model.addNode("Node1");
    NodeId node2 = model.addNode("Node2");
    NodeId node3 = model.addNode("Node3");

    ConnectionId conn1{node1, 0, node2, 0};
    ConnectionId conn2{node2, 0, node3, 0};

    model.addConnection(conn1);
    model.addConnection(conn2);

    CHECK(model.connectionExists(conn1));
    CHECK(model.connectionExists(conn2));

    // Delete node2, which should remove both connections
    model.deleteNode(node2);

    CHECK_FALSE(model.nodeExists(node2));
    CHECK_FALSE(model.connectionExists(conn1));
    CHECK_FALSE(model.connectionExists(conn2));

    // Node1 and node3 should still exist
    CHECK(model.nodeExists(node1));
    CHECK(model.nodeExists(node3));
}
