#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <QtNodes/AbstractGraphModel>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>
#include <QSignalSpy>
#include <QMetaType>

using QtNodes::ConnectionId;
using QtNodes::InvalidNodeId;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortType;

// Register ConnectionId as meta type for signal testing
Q_DECLARE_METATYPE(QtNodes::ConnectionId)

TEST_CASE("AbstractGraphModel signal emissions", "[signals]")
{
    auto app = applicationSetup();
    
    // Register meta types for signal testing
    qRegisterMetaType<QtNodes::ConnectionId>("ConnectionId");
    qRegisterMetaType<QtNodes::NodeId>("NodeId");
    
    TestGraphModel model;

    SECTION("Node creation signals")
    {
        QSignalSpy nodeCreatedSpy(&model, &TestGraphModel::nodeCreated);
        
        NodeId nodeId = model.addNode("TestNode");
        
        REQUIRE(nodeCreatedSpy.count() == 1);
        QList<QVariant> arguments = nodeCreatedSpy.takeFirst();
        CHECK(arguments.at(0).value<NodeId>() == nodeId);
        CHECK(nodeId != InvalidNodeId);
    }

    SECTION("Node deletion signals")
    {
        // Create a node first
        NodeId nodeId = model.addNode("TestNode");
        
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        
        bool deleted = model.deleteNode(nodeId);
        
        REQUIRE(deleted);
        REQUIRE(nodeDeletedSpy.count() == 1);
        QList<QVariant> arguments = nodeDeletedSpy.takeFirst();
        CHECK(arguments.at(0).value<NodeId>() == nodeId);
    }

    SECTION("Node update signals")
    {
        NodeId nodeId = model.addNode("TestNode");
        
        QSignalSpy nodeUpdatedSpy(&model, &TestGraphModel::nodeUpdated);
        QSignalSpy nodePositionUpdatedSpy(&model, &TestGraphModel::nodePositionUpdated);
        
        // Test position update signal
        QPointF newPosition(100.0, 200.0);
        bool positionSet = model.setNodeData(nodeId, NodeRole::Position, newPosition);
        
        CHECK(positionSet);
        CHECK(nodePositionUpdatedSpy.count() == 1);
        QList<QVariant> posArgs = nodePositionUpdatedSpy.takeFirst();
        CHECK(posArgs.at(0).value<NodeId>() == nodeId);
        
        // Test general node update signal (for non-position changes)
        bool captionSet = model.setNodeData(nodeId, NodeRole::Caption, QString("New Caption"));
        
        CHECK(captionSet);
        CHECK(nodeUpdatedSpy.count() == 1);
        QList<QVariant> updateArgs = nodeUpdatedSpy.takeFirst();
        CHECK(updateArgs.at(0).value<NodeId>() == nodeId);
    }

    SECTION("Connection creation signals")
    {
        // Create two nodes
        NodeId node1 = model.addNode("TestNode");
        NodeId node2 = model.addNode("TestNode");
        
        QSignalSpy connectionCreatedSpy(&model, &TestGraphModel::connectionCreated);
        
        ConnectionId connId{node1, 0, node2, 0};
        model.addConnection(connId);
        
        REQUIRE(connectionCreatedSpy.count() == 1);
        QList<QVariant> arguments = connectionCreatedSpy.takeFirst();
        ConnectionId emittedConnId = arguments.at(0).value<ConnectionId>();
        CHECK(emittedConnId.outNodeId == node1);
        CHECK(emittedConnId.outPortIndex == 0);
        CHECK(emittedConnId.inNodeId == node2);
        CHECK(emittedConnId.inPortIndex == 0);
    }

    SECTION("Connection deletion signals")
    {
        // Create two nodes and a connection
        NodeId node1 = model.addNode("TestNode");
        NodeId node2 = model.addNode("TestNode");
        ConnectionId connId{node1, 0, node2, 0};
        model.addConnection(connId);
        
        QSignalSpy connectionDeletedSpy(&model, &TestGraphModel::connectionDeleted);
        
        bool deleted = model.deleteConnection(connId);
        
        REQUIRE(deleted);
        REQUIRE(connectionDeletedSpy.count() == 1);
        QList<QVariant> arguments = connectionDeletedSpy.takeFirst();
        ConnectionId emittedConnId = arguments.at(0).value<ConnectionId>();
        CHECK(emittedConnId.outNodeId == node1);
        CHECK(emittedConnId.outPortIndex == 0);
        CHECK(emittedConnId.inNodeId == node2);
        CHECK(emittedConnId.inPortIndex == 0);
    }

    SECTION("Multiple signal emissions for node deletion with connections")
    {
        // Create nodes and connections
        NodeId node1 = model.addNode("TestNode");
        NodeId node2 = model.addNode("TestNode");
        NodeId node3 = model.addNode("TestNode");
        
        ConnectionId conn1{node1, 0, node2, 0};
        ConnectionId conn2{node1, 0, node3, 0};
        model.addConnection(conn1);
        model.addConnection(conn2);
        
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        QSignalSpy connectionDeletedSpy(&model, &TestGraphModel::connectionDeleted);
        
        // Delete node1 - should emit signals for deleted connections and node
        bool deleted = model.deleteNode(node1);
        
        REQUIRE(deleted);
        
        // Should have deleted 2 connections and 1 node
        CHECK(connectionDeletedSpy.count() == 2);
        CHECK(nodeDeletedSpy.count() == 1);
        
        // Verify node deletion signal
        QList<QVariant> nodeArgs = nodeDeletedSpy.takeFirst();
        CHECK(nodeArgs.at(0).value<NodeId>() == node1);
    }

    SECTION("Signal emission order for complex operations")
    {
        NodeId node1 = model.addNode("TestNode");
        NodeId node2 = model.addNode("TestNode");
        
        QSignalSpy nodeCreatedSpy(&model, &TestGraphModel::nodeCreated);
        QSignalSpy connectionCreatedSpy(&model, &TestGraphModel::connectionCreated);
        QSignalSpy connectionDeletedSpy(&model, &TestGraphModel::connectionDeleted);
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        
        // Reset spy counts (nodes were already created above)
        nodeCreatedSpy.clear();
        
        // Create connection
        ConnectionId connId{node1, 0, node2, 0};
        model.addConnection(connId);
        
        CHECK(connectionCreatedSpy.count() == 1);
        
        // Delete connection
        model.deleteConnection(connId);
        
        CHECK(connectionDeletedSpy.count() == 1);
        
        // Delete nodes
        model.deleteNode(node1);
        model.deleteNode(node2);
        
        CHECK(nodeDeletedSpy.count() == 2);
    }
}

TEST_CASE("AbstractGraphModel signal spy validation", "[signals]")
{
    auto app = applicationSetup();
    
    // Register meta types for signal testing
    qRegisterMetaType<QtNodes::ConnectionId>("ConnectionId");
    qRegisterMetaType<QtNodes::NodeId>("NodeId");
    
    TestGraphModel model;

    SECTION("Signal spy basic functionality")
    {
        QSignalSpy nodeCreatedSpy(&model, &TestGraphModel::nodeCreated);
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        
        CHECK(nodeCreatedSpy.isValid());
        CHECK(nodeDeletedSpy.isValid());
        
        // Verify no signals emitted initially
        CHECK(nodeCreatedSpy.count() == 0);
        CHECK(nodeDeletedSpy.count() == 0);
    }

    SECTION("Signal argument types")
    {
        QSignalSpy nodeCreatedSpy(&model, &TestGraphModel::nodeCreated);
        QSignalSpy connectionCreatedSpy(&model, &TestGraphModel::connectionCreated);
        
        NodeId node1 = model.addNode("TestNode");
        NodeId node2 = model.addNode("TestNode");
        ConnectionId connId{node1, 0, node2, 0};
        model.addConnection(connId);
        
        // Check signal argument types
        REQUIRE(nodeCreatedSpy.count() >= 1);
        QList<QVariant> nodeArgs = nodeCreatedSpy.takeFirst();
        CHECK(nodeArgs.size() == 1);
        CHECK(nodeArgs.at(0).userType() == QMetaType::UInt); // NodeId is unsigned int
        
        REQUIRE(connectionCreatedSpy.count() == 1);
        QList<QVariant> connArgs = connectionCreatedSpy.takeFirst();
        CHECK(connArgs.size() == 1);
        // ConnectionId should be registered as a custom type
        CHECK(connArgs.at(0).canConvert<ConnectionId>());
    }
}

TEST_CASE("AbstractGraphModel edge case signal emissions", "[signals]")
{
    auto app = applicationSetup();
    
    // Register meta types for signal testing
    qRegisterMetaType<QtNodes::ConnectionId>("ConnectionId");
    qRegisterMetaType<QtNodes::NodeId>("NodeId");
    
    TestGraphModel model;

    SECTION("No signals for invalid operations")
    {
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        QSignalSpy connectionDeletedSpy(&model, &TestGraphModel::connectionDeleted);
        
        // Try to delete non-existent node
        bool deleted = model.deleteNode(999999);
        CHECK_FALSE(deleted);
        CHECK(nodeDeletedSpy.count() == 0);
        
        // Try to delete non-existent connection
        ConnectionId invalidConn{999999, 0, 999998, 0};
        bool connDeleted = model.deleteConnection(invalidConn);
        CHECK_FALSE(connDeleted);
        CHECK(connectionDeletedSpy.count() == 0);
    }

    SECTION("Signal consistency with model state")
    {
        QSignalSpy nodeCreatedSpy(&model, &TestGraphModel::nodeCreated);
        
        NodeId nodeId = model.addNode("TestNode");
        
        // Verify signal was emitted
        REQUIRE(nodeCreatedSpy.count() == 1);
        
        // Verify model state matches signal
        CHECK(model.nodeExists(nodeId));
        CHECK(model.allNodeIds().count(nodeId) == 1);
        
        QSignalSpy nodeDeletedSpy(&model, &TestGraphModel::nodeDeleted);
        
        model.deleteNode(nodeId);
        
        // Verify signal was emitted
        REQUIRE(nodeDeletedSpy.count() == 1);
        
        // Verify model state matches signal
        CHECK_FALSE(model.nodeExists(nodeId));
        CHECK(model.allNodeIds().count(nodeId) == 0);
    }
}
