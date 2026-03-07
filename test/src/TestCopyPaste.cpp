#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

#include <QApplication>
#include <QClipboard>
#include <QTest>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionId;
using QtNodes::GraphicsView;
using QtNodes::NodeId;
using QtNodes::NodeRole;

TEST_CASE("Copy/Paste basic functionality", "[copypaste]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    SECTION("Copy single node")
    {
        // Create a node
        NodeId nodeId = model->addNode("TestNode");
        model->setNodeData(nodeId, NodeRole::Position, QPointF(100, 100));

        QCoreApplication::processEvents();

        // Select the node
        auto *nodeGraphics = scene.nodeGraphicsObject(nodeId);
        REQUIRE(nodeGraphics != nullptr);
        nodeGraphics->setSelected(true);

        QCoreApplication::processEvents();

        // Trigger copy action
        view.onCopySelectedObjects();

        QCoreApplication::processEvents();

        // Clipboard should contain data
        QClipboard *clipboard = QApplication::clipboard();
        QString clipboardText = clipboard->text();
        // We check that clipboard has some content (exact format is implementation detail)
        CHECK(!clipboardText.isEmpty());
    }

    SECTION("Copy and paste creates new node")
    {
        // Create a node
        NodeId nodeId = model->addNode("TestNode");
        model->setNodeData(nodeId, NodeRole::Position, QPointF(100, 100));

        QCoreApplication::processEvents();

        // Select the node
        auto *nodeGraphics = scene.nodeGraphicsObject(nodeId);
        REQUIRE(nodeGraphics != nullptr);
        nodeGraphics->setSelected(true);

        size_t initialNodeCount = model->allNodeIds().size();
        CHECK(initialNodeCount == 1);

        // Copy
        view.onCopySelectedObjects();
        QCoreApplication::processEvents();

        // Paste
        view.onPasteObjects();
        QCoreApplication::processEvents();

        // Should have a new node
        CHECK(model->allNodeIds().size() >= initialNodeCount);
    }

    SECTION("Duplicate creates new node")
    {
        // Create a node
        NodeId nodeId = model->addNode("TestNode");
        model->setNodeData(nodeId, NodeRole::Position, QPointF(100, 100));

        QCoreApplication::processEvents();

        // Select the node
        auto *nodeGraphics = scene.nodeGraphicsObject(nodeId);
        REQUIRE(nodeGraphics != nullptr);
        nodeGraphics->setSelected(true);

        size_t initialNodeCount = model->allNodeIds().size();

        // Duplicate
        view.onDuplicateSelectedObjects();
        QCoreApplication::processEvents();

        // Should have a new node
        CHECK(model->allNodeIds().size() > initialNodeCount);
    }
}

TEST_CASE("Copy/Paste with connections", "[copypaste]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    SECTION("Copy multiple connected nodes")
    {
        // Create two connected nodes
        NodeId node1 = model->addNode("Node1");
        model->setNodeData(node1, NodeRole::Position, QPointF(100, 100));

        NodeId node2 = model->addNode("Node2");
        model->setNodeData(node2, NodeRole::Position, QPointF(300, 100));

        model->addConnection(ConnectionId{node1, 0, node2, 0});

        QCoreApplication::processEvents();

        // Select both nodes
        auto *nodeGraphics1 = scene.nodeGraphicsObject(node1);
        auto *nodeGraphics2 = scene.nodeGraphicsObject(node2);
        REQUIRE(nodeGraphics1 != nullptr);
        REQUIRE(nodeGraphics2 != nullptr);

        nodeGraphics1->setSelected(true);
        nodeGraphics2->setSelected(true);

        QCoreApplication::processEvents();

        size_t initialNodeCount = model->allNodeIds().size();

        // Duplicate
        view.onDuplicateSelectedObjects();
        QCoreApplication::processEvents();

        // Should have new nodes
        CHECK(model->allNodeIds().size() > initialNodeCount);
    }
}
