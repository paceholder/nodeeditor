#include "ApplicationSetup.hpp"

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::InvalidNodeId;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::NodeRole;

class SerializableTestModel : public NodeDelegateModel
{
public:
    QString name() const override { return "SerializableTestModel"; }
    QString caption() const override { return "Test Model for Serialization"; }
    
    unsigned int nPorts(QtNodes::PortType portType) const override
    {
        return (portType == QtNodes::PortType::In) ? 1 : 1;
    }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return {}; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const) override {}
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const) override { return nullptr; }
    QWidget* embeddedWidget() override { return nullptr; }
};

TEST_CASE("DataFlowGraphModel serialization", "[serialization]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<SerializableTestModel>("SerializableTestModel");

    DataFlowGraphModel model(registry);

    SECTION("Save and load empty model")
    {
        QJsonObject json = model.save();
        CHECK_FALSE(json.isEmpty());
        
        // Should have nodes and connections arrays
        CHECK(json.contains("nodes"));
        CHECK(json.contains("connections"));
        CHECK(json["nodes"].isArray());
        CHECK(json["connections"].isArray());
        
        // Arrays should be empty for empty model
        CHECK(json["nodes"].toArray().size() == 0);
        CHECK(json["connections"].toArray().size() == 0);
    }

    SECTION("Save and load model with nodes")
    {
        // Create nodes
        NodeId node1 = model.addNode("SerializableTestModel");
        NodeId node2 = model.addNode("SerializableTestModel");
        
        // Validate nodes were created successfully
        CHECK(node1 != InvalidNodeId);
        CHECK(node2 != InvalidNodeId);
        
        // Set positions
        model.setNodeData(node1, NodeRole::Position, QPointF(100, 200));
        model.setNodeData(node2, NodeRole::Position, QPointF(300, 400));
        
        // Save
        QJsonObject json = model.save();
        
        CHECK(json["nodes"].toArray().size() == 2);
        CHECK(json["connections"].toArray().size() == 0);
        
        // Create new model and load
        DataFlowGraphModel newModel(registry);
        newModel.load(json);
        
        // Check that nodes were loaded
        auto nodeIds = newModel.allNodeIds();
        CHECK(nodeIds.size() == 2);
        
        // Note: Node IDs might be different after loading, but positions should be preserved
        for (NodeId id : nodeIds) {
            QPointF pos = newModel.nodeData(id, NodeRole::Position).toPointF();
            CHECK((pos == QPointF(100, 200) || pos == QPointF(300, 400)));
        }
    }

    SECTION("Save and load model with connections")
    {
        // Create nodes and connection
        NodeId node1 = model.addNode("SerializableTestModel");
        NodeId node2 = model.addNode("SerializableTestModel");
        
        // Validate nodes were created successfully
        CHECK(node1 != InvalidNodeId);
        CHECK(node2 != InvalidNodeId);
        
        ConnectionId conn{node1, 0, node2, 0};
        model.addConnection(conn);
        
        // Save
        QJsonObject json = model.save();
        
        CHECK(json["nodes"].toArray().size() == 2);
        CHECK(json["connections"].toArray().size() == 1);
        
        // Create new model and load
        DataFlowGraphModel newModel(registry);
        newModel.load(json);
        
        // Check that connection was loaded
        auto nodeIds = newModel.allNodeIds();
        CHECK(nodeIds.size() == 2);
        
        // Find a node that has connections
        bool foundConnection = false;
        for (NodeId id : nodeIds) {
            auto connections = newModel.allConnectionIds(id);
            if (!connections.empty()) {
                foundConnection = true;
                break;
            }
        }
        CHECK(foundConnection);
    }
}

TEST_CASE("Individual node serialization", "[serialization]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<SerializableTestModel>("SerializableTestModel");

    DataFlowGraphModel model(registry);

    SECTION("Save individual node")
    {
        NodeId nodeId = model.addNode("SerializableTestModel");
        CHECK(nodeId != InvalidNodeId);
        
        model.setNodeData(nodeId, NodeRole::Position, QPointF(150, 250));
        
        QJsonObject nodeJson = model.saveNode(nodeId);
        CHECK_FALSE(nodeJson.isEmpty());
        
        // Should contain at least some node information
        CHECK(nodeJson.contains("id"));
        CHECK(nodeJson.contains("position"));
    }
}
