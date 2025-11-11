#include "ApplicationSetup.hpp"

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/Definitions>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/NodeGroup.hpp>

#include <catch2/catch.hpp>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>

#include <algorithm>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::GroupId;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeGroup;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace {
class DummyNodeModel : public NodeDelegateModel
{
public:
    QString caption() const override { return QStringLiteral("Dummy Node"); }
    QString name() const override { return QStringLiteral("DummyNode"); }

    unsigned int nPorts(PortType portType) const override
    {
        Q_UNUSED(portType);
        return 1U;
    }

    QtNodes::NodeDataType dataType(PortType, PortIndex) const override { return {}; }

    std::shared_ptr<QtNodes::NodeData> outData(PortIndex const) override { return nullptr; }

    void setInData(std::shared_ptr<QtNodes::NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};

std::shared_ptr<NodeDelegateModelRegistry> createDummyRegistry()
{
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<DummyNodeModel>(QStringLiteral("Test"));
    return registry;
}

NodeId createNode(DataFlowGraphModel &model, BasicGraphicsScene &scene)
{
    NodeId nodeId = model.addNode(QStringLiteral("DummyNode"));
    REQUIRE(nodeId != QtNodes::InvalidNodeId);

    QCoreApplication::processEvents();

    auto *nodeObject = scene.nodeGraphicsObject(nodeId);
    REQUIRE(nodeObject != nullptr);

    return nodeId;
}

std::set<NodeId> toNodeIdSet(std::vector<NodeId> const &ids)
{
    return {ids.begin(), ids.end()};
}

} // namespace

TEST_CASE("Node group creation", "[node-group]")
{
    auto app = applicationSetup();

    auto registry = createDummyRegistry();
    DataFlowGraphModel model(registry);
    BasicGraphicsScene scene(model);
    scene.setGroupingEnabled(true);

    SECTION("Creating a group from a single node")
    {
        NodeId nodeId = createNode(model, scene);
        auto *nodeObject = scene.nodeGraphicsObject(nodeId);
        REQUIRE(nodeObject != nullptr);

        std::vector<NodeGraphicsObject *> nodes{nodeObject};
        auto groupWeak = scene.createGroup(nodes, QStringLiteral("SingleGroup"));
        auto group = groupWeak.lock();

        REQUIRE(group);
        CHECK(scene.groups().size() == 1);
        REQUIRE(group->childNodes().size() == 1);
        CHECK(group->childNodes().front()->nodeId() == nodeId);

        auto groupIds = group->nodeIDs();
        REQUIRE(groupIds.size() == 1);
        CHECK(groupIds.front() == nodeId);

        auto nodeGroup = nodeObject->nodeGroup().lock();
        REQUIRE(nodeGroup);
        CHECK(nodeGroup->id() == group->id());
    }

    SECTION("Creating multiple groups and verifying membership")
    {
        constexpr std::size_t nodesPerGroup = 3;
        constexpr std::size_t groupCount = 2;

        std::vector<std::vector<NodeId>> expectedNodeIds(groupCount);
        std::vector<GroupId> groupIds;

        for (std::size_t groupIndex = 0; groupIndex < groupCount; ++groupIndex) {
            std::vector<NodeGraphicsObject *> nodeObjects;
            nodeObjects.reserve(nodesPerGroup);
            expectedNodeIds[groupIndex].reserve(nodesPerGroup);

            for (std::size_t i = 0; i < nodesPerGroup; ++i) {
                NodeId nodeId = createNode(model, scene);
                expectedNodeIds[groupIndex].push_back(nodeId);
                nodeObjects.push_back(scene.nodeGraphicsObject(nodeId));
            }

            auto groupWeak = scene.createGroup(nodeObjects,
                                               QStringLiteral("Group%1").arg(groupIndex));
            auto group = groupWeak.lock();
            REQUIRE(group);
            groupIds.push_back(group->id());
        }

        CHECK(scene.groups().size() == groupCount);

        for (std::size_t index = 0; index < groupIds.size(); ++index) {
            auto const &groupId = groupIds[index];
            auto groupIt = scene.groups().find(groupId);
            REQUIRE(groupIt != scene.groups().end());

            auto const &group = groupIt->second;
            REQUIRE(group);

            auto const nodeIds = group->nodeIDs();
            CHECK(toNodeIdSet(nodeIds) == toNodeIdSet(expectedNodeIds[index]));

            for (auto *node : group->childNodes()) {
                REQUIRE(node);
                auto nodeGroup = node->nodeGroup().lock();
                REQUIRE(nodeGroup);
                CHECK(nodeGroup->id() == groupId);
            }
        }
    }
}

TEST_CASE("Adding and removing nodes from a group", "[node-group]")
{
    auto app = applicationSetup();

    auto registry = createDummyRegistry();
    DataFlowGraphModel model(registry);
    BasicGraphicsScene scene(model);
    scene.setGroupingEnabled(true);

    SECTION("Adding a node to an existing group")
    {
        NodeId firstNodeId = createNode(model, scene);
        auto *firstNode = scene.nodeGraphicsObject(firstNodeId);
        REQUIRE(firstNode != nullptr);

        std::vector<NodeGraphicsObject *> nodes{firstNode};
        auto group = scene.createGroup(nodes, QStringLiteral("ExtendableGroup")).lock();
        REQUIRE(group);

        NodeId extraNodeId = createNode(model, scene);
        auto *extraNode = scene.nodeGraphicsObject(extraNodeId);
        REQUIRE(extraNode != nullptr);

        scene.addNodeToGroup(extraNodeId, group->id());

        auto const groupIds = group->nodeIDs();
        CHECK(groupIds.size() == 2);
        CHECK(std::find(groupIds.begin(), groupIds.end(), extraNodeId) != groupIds.end());

        auto nodeGroup = extraNode->nodeGroup().lock();
        REQUIRE(nodeGroup);
        CHECK(nodeGroup->id() == group->id());
    }

    SECTION("Removing nodes from a group and clearing empty groups")
    {
        std::vector<NodeGraphicsObject *> nodes;
        std::vector<NodeId> nodeIds;
        nodes.reserve(2);
        nodeIds.reserve(2);

        for (int i = 0; i < 2; ++i) {
            NodeId id = createNode(model, scene);
            nodeIds.push_back(id);
            nodes.push_back(scene.nodeGraphicsObject(id));
        }

        auto group = scene.createGroup(nodes, QStringLiteral("RemovableGroup")).lock();
        REQUIRE(group);
        auto groupId = group->id();

        scene.removeNodeFromGroup(nodeIds.front());
        auto const remainingIds = group->nodeIDs();
        CHECK(std::find(remainingIds.begin(), remainingIds.end(), nodeIds.front())
              == remainingIds.end());

        auto *removedNode = scene.nodeGraphicsObject(nodeIds.front());
        REQUIRE(removedNode != nullptr);
        CHECK(removedNode->nodeGroup().expired());

        scene.removeNodeFromGroup(nodeIds.back());
        CHECK(scene.groups().find(groupId) == scene.groups().end());
    }

    SECTION("Deleting grouped nodes updates the scene")
    {
        std::vector<NodeGraphicsObject *> nodes;
        std::vector<NodeId> nodeIds;
        nodes.reserve(2);
        nodeIds.reserve(2);

        for (int i = 0; i < 2; ++i) {
            NodeId id = createNode(model, scene);
            nodeIds.push_back(id);
            nodes.push_back(scene.nodeGraphicsObject(id));
        }

        auto group = scene.createGroup(nodes, QStringLiteral("DeletionGroup")).lock();
        REQUIRE(group);
        auto groupId = group->id();

        model.deleteNode(nodeIds.front());
        QCoreApplication::processEvents();

        CHECK(scene.groups().find(groupId) != scene.groups().end());

        model.deleteNode(nodeIds.back());
        QCoreApplication::processEvents();

        CHECK(scene.groups().find(groupId) == scene.groups().end());
        CHECK(scene.nodeGraphicsObject(nodeIds.front()) == nullptr);
        CHECK(scene.nodeGraphicsObject(nodeIds.back()) == nullptr);
    }
}

TEST_CASE("Saving and restoring node groups", "[node-group]")
{
    auto app = applicationSetup();

    auto registry = createDummyRegistry();
    DataFlowGraphModel model(registry);
    BasicGraphicsScene scene(model);
    scene.setGroupingEnabled(true);

    SECTION("Saving a group serializes nodes and connections")
    {
        std::vector<NodeGraphicsObject *> nodeObjects;
        std::vector<NodeId> nodeIds;
        nodeObjects.reserve(2);
        nodeIds.reserve(2);

        for (int i = 0; i < 2; ++i) {
            NodeId nodeId = createNode(model, scene);
            nodeIds.push_back(nodeId);
            nodeObjects.push_back(scene.nodeGraphicsObject(nodeId));
            model.setNodeData(nodeId, NodeRole::Position, QPointF(100.0 * i, 50.0 * i));
        }

        auto group = scene.createGroup(nodeObjects, QStringLiteral("SerializableGroup")).lock();
        REQUIRE(group);

        ConnectionId connection{nodeIds[0], 0, nodeIds[1], 0};
        model.addConnection(connection);
        QCoreApplication::processEvents();

        auto groupJson = QJsonDocument::fromJson(group->saveToFile()).object();
        CHECK(groupJson["name"].toString() == QStringLiteral("SerializableGroup"));
        CHECK(static_cast<GroupId>(groupJson["id"].toInt()) == group->id());

        auto nodesJson = groupJson["nodes"].toArray();
        CHECK(nodesJson.size() == 2);

        std::set<NodeId> serializedIds;
        for (auto const &nodeValue : nodesJson) {
            auto nodeObject = nodeValue.toObject();
            NodeId serializedId = static_cast<NodeId>(nodeObject["id"].toInt());
            serializedIds.insert(serializedId);
            CHECK(nodeObject.contains("position"));
        }
        CHECK(serializedIds == toNodeIdSet(nodeIds));

        auto connectionsJson = groupJson["connections"].toArray();
        CHECK(connectionsJson.size() == 1);

        auto connectionObject = connectionsJson.first().toObject();
        CHECK(static_cast<NodeId>(connectionObject["outNodeId"].toInt()) == nodeIds[0]);
        CHECK(static_cast<NodeId>(connectionObject["intNodeId"].toInt()) == nodeIds[1]);
    }

    SECTION("Restoring a group from serialized data")
    {
        std::vector<NodeGraphicsObject *> nodeObjects;
        std::vector<NodeId> nodeIds;
        nodeObjects.reserve(2);
        nodeIds.reserve(2);

        for (int i = 0; i < 2; ++i) {
            NodeId nodeId = createNode(model, scene);
            nodeIds.push_back(nodeId);
            nodeObjects.push_back(scene.nodeGraphicsObject(nodeId));
            model.setNodeData(nodeId, NodeRole::Position, QPointF(150.0 * i, 60.0 * i));
        }

        auto group = scene.createGroup(nodeObjects, QStringLiteral("OriginalGroup")).lock();
        REQUIRE(group);

        ConnectionId connection{nodeIds[0], 0, nodeIds[1], 0};
        model.addConnection(connection);
        QCoreApplication::processEvents();

        auto groupJson = QJsonDocument::fromJson(group->saveToFile()).object();

        auto newRegistry = createDummyRegistry();
        DataFlowGraphModel newModel(newRegistry);
        BasicGraphicsScene newScene(newModel);
        newScene.setGroupingEnabled(true);

        auto [restoredGroupWeak, idMapping] = newScene.restoreGroup(groupJson);
        auto restoredGroup = restoredGroupWeak.lock();
        REQUIRE(restoredGroup);

        CHECK(newScene.groups().find(restoredGroup->id()) != newScene.groups().end());

        auto restoredIds = restoredGroup->nodeIDs();
        CHECK(restoredIds.size() == nodeIds.size());

        for (auto originalId : nodeIds) {
            auto mappingIt = idMapping.find(static_cast<GroupId>(originalId));
            REQUIRE(mappingIt != idMapping.end());
            NodeId restoredId = static_cast<NodeId>(mappingIt->second);
            CHECK(std::find(restoredIds.begin(), restoredIds.end(), restoredId)
                  != restoredIds.end());
        }

        REQUIRE_FALSE(restoredIds.empty());
        auto connections = newModel.allConnectionIds(restoredIds.front());
        REQUIRE_FALSE(connections.empty());

        auto connectionIt = connections.begin();
        std::set<NodeId> restoredSet(restoredIds.begin(), restoredIds.end());
        CHECK(restoredSet.count(connectionIt->outNodeId) == 1);
        CHECK(restoredSet.count(connectionIt->inNodeId) == 1);
    }
}
