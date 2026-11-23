#include "ApplicationSetup.hpp"
#include "TestDelegateModels.hpp"

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/Definitions>
#include <QtNodes/internal/ConnectionIdUtils.hpp>

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

namespace {

std::unique_ptr<NodeDelegateModel> createSerializableModel()
{
    return std::make_unique<StubDelegateModel>("SerializableTestModel",
                                               "Test Model for Serialization", 1, 1);
}

} // namespace

TEST_CASE("DataFlowGraphModel serialization", "[serialization]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel(createSerializableModel, "SerializableTestModel");

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
    registry->registerModel(createSerializableModel, "SerializableTestModel");

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

TEST_CASE("DataFlowGraphModel loads node ids above the signed 32-bit range", "[serialization]")
{
    auto app = applicationSetup();
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel(createSerializableModel, "SerializableTestModel");

    DataFlowGraphModel model(registry);
    NodeId const a = model.addNode("SerializableTestModel");
    NodeId const b = model.addNode("SerializableTestModel");
    model.addConnection(ConnectionId{a, 0, b, 0});

    quint32 const bigA = 3000000000u;
    quint32 const bigB = 3000000001u;

    QJsonObject json = model.save();

    QJsonArray nodes = json["nodes"].toArray();
    for (int i = 0; i < nodes.size(); ++i) {
        QJsonObject node = nodes[i].toObject();
        node["id"] = static_cast<qint64>(node["id"].toInt() == static_cast<int>(a) ? bigA : bigB);
        nodes[i] = node;
    }
    json["nodes"] = nodes;

    QJsonObject conn = QtNodes::toJson(ConnectionId{bigA, 0, bigB, 0});
    json["connections"] = QJsonArray{conn};

    DataFlowGraphModel loaded(registry);
    loaded.load(json);

    CHECK(loaded.nodeExists(bigA));
    CHECK(loaded.nodeExists(bigB));
    CHECK(loaded.connectionExists(ConnectionId{bigA, 0, bigB, 0}));
}
