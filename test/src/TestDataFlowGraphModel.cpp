#include "ApplicationSetup.hpp"

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::InvalidNodeId;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortType;

class TestNodeDelegate : public NodeDelegateModel
{
public:
    QString name() const override { return "TestNode"; }
    QString caption() const override { return "Test Node"; }
    unsigned int nPorts(QtNodes::PortType portType) const override
    {
        return (portType == PortType::In) ? 2 : 1;
    }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return {}; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const) override {}
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const) override { return nullptr; }
    QWidget* embeddedWidget() override { return nullptr; }
};

TEST_CASE("DataFlowGraphModel basic functionality", "[dataflow]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<TestNodeDelegate>("TestNode");

    DataFlowGraphModel model(registry);

    SECTION("Node creation with delegate")
    {
        CHECK(model.allNodeIds().empty());

        NodeId nodeId = model.addNode("TestNode");
        CHECK(nodeId != InvalidNodeId);
        CHECK(model.nodeExists(nodeId));
        CHECK(model.allNodeIds().size() == 1);

        // Check node data is properly set
        CHECK(model.nodeData(nodeId, NodeRole::Type).toString() == "TestNode");
        CHECK(model.nodeData(nodeId, NodeRole::Caption).toString() == "Test Node");
        CHECK(model.nodeData(nodeId, NodeRole::InPortCount).toUInt() == 2);
        CHECK(model.nodeData(nodeId, NodeRole::OutPortCount).toUInt() == 1);
    }

    SECTION("Invalid node type")
    {
        // Trying to create a node with unregistered type should fail
        // and return InvalidNodeId
        NodeId nodeId = model.addNode("NonExistentType");
        CHECK(nodeId == InvalidNodeId);
        CHECK_FALSE(model.nodeExists(nodeId));
    }

    SECTION("Registry access")
    {
        auto retrievedRegistry = model.dataModelRegistry();
        CHECK(retrievedRegistry == registry);
        CHECK(retrievedRegistry != nullptr);
    }
}

TEST_CASE("DataFlowGraphModel connections", "[dataflow]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<TestNodeDelegate>("TestNode");

    DataFlowGraphModel model(registry);

    NodeId node1 = model.addNode("TestNode");
    NodeId node2 = model.addNode("TestNode");

    SECTION("Valid connection between delegate nodes")
    {
        ConnectionId connId{node1, 0, node2, 0};

        CHECK(model.connectionPossible(connId));
        model.addConnection(connId);
        CHECK(model.connectionExists(connId));

        auto connections = model.connections(node1, PortType::Out, 0);
        CHECK(connections.size() == 1);
        CHECK(connections.count(connId) == 1);
    }

    SECTION("Connection validation with port bounds")
    {
        // Valid ports (TestNode has 1 output, 2 inputs)
        ConnectionId validConn{node1, 0, node2, 0};
        CHECK(model.connectionPossible(validConn));

        ConnectionId validConn2{node1, 0, node2, 1};
        CHECK(model.connectionPossible(validConn2));

        // Invalid output port (only has port 0)
        ConnectionId invalidOut{node1, 1, node2, 0};
        CHECK_FALSE(model.connectionPossible(invalidOut));

        // Invalid input port (only has ports 0 and 1)
        ConnectionId invalidIn{node1, 0, node2, 2};
        CHECK_FALSE(model.connectionPossible(invalidIn));
    }

    SECTION("Loop connection between three nodes")
    {
      NodeId node3 = model.addNode("TestNode");

      ConnectionId connId12{node1, 0, node2, 0};

      CHECK(model.connectionPossible(connId12));
      model.addConnection(connId12);
      CHECK(model.connectionExists(connId12));

      ConnectionId connId23{node2, 0, node3, 0};

      CHECK(model.connectionPossible(connId23));
      model.addConnection(connId23);
      CHECK(model.connectionExists(connId23));

      ConnectionId connId31{node3, 0, node1, 0};

      CHECK_FALSE(model.connectionPossible(connId31));
      model.addConnection(connId31);
      CHECK(model.connectionExists(connId31));
    }
}

TEST_CASE("DataFlowGraphModel serialization support", "[dataflow]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<TestNodeDelegate>("TestNode");

    DataFlowGraphModel model(registry);

    NodeId node1 = model.addNode("TestNode");
    NodeId node2 = model.addNode("TestNode");
    
    model.setNodeData(node1, NodeRole::Position, QPointF(0, 0));
    model.setNodeData(node2, NodeRole::Position, QPointF(100, 100));

    ConnectionId connId{node1, 0, node2, 0};
    model.addConnection(connId);

    SECTION("Save and load operations exist")
    {
        // These should not throw and should return valid JSON
        QJsonObject nodeJson = model.saveNode(node1);
        QJsonObject fullJson = model.save();
        
        // Basic validation that something was saved
        CHECK_FALSE(nodeJson.isEmpty());
        CHECK_FALSE(fullJson.isEmpty());
        CHECK(fullJson.contains("nodes"));
        CHECK(fullJson.contains("connections"));
    }
}
