#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"
#include "UITestHelper.hpp"

#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>
#include <QSignalSpy>
#include <QUndoStack>

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeId;
using QtNodes::NodeRole;

TEST_CASE("Interactive node drag syncs the model", "[ui][visual][drag]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);
    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    NodeId const nodeId = model->addNode("TestNode");
    model->setNodeData(nodeId, NodeRole::Position, QPointF(0, 0));
    UITestHelper::waitForUI();

    auto *item = scene.nodeGraphicsObject(nodeId);
    REQUIRE(item != nullptr);

    // No style is loaded in tests, so the default opacity is 0 and Qt would
    // skip the node when hit-testing the simulated mouse events.
    item->setOpacity(1.0);

    QPointF const start = item->mapToScene(item->boundingRect().center());
    QPointF const delta(60, 40);

    SECTION("Dragging writes the new position to the model and emits nodeMoved")
    {
        QSignalSpy movedSpy(&scene, &BasicGraphicsScene::nodeMoved);

        UITestHelper::simulateMouseDrag(&view, start, start + delta);

        CHECK(item->pos() == QPointF(0, 0) + delta);
        CHECK(model->nodeData(nodeId, NodeRole::Position).value<QPointF>() == item->pos());
        REQUIRE(movedSpy.count() == 1);
        CHECK(movedSpy.at(0).at(1).toPointF() == item->pos());
    }

    SECTION("A drag is a single undo step")
    {
        UITestHelper::simulateMouseDrag(&view, start, start + delta);
        REQUIRE(scene.undoStack().count() == 1);

        scene.undoStack().undo();
        CHECK(model->nodeData(nodeId, NodeRole::Position).value<QPointF>() == QPointF(0, 0));
        CHECK(item->pos() == QPointF(0, 0));

        scene.undoStack().redo();
        CHECK(model->nodeData(nodeId, NodeRole::Position).value<QPointF>() == delta);
        CHECK(item->pos() == delta);
    }

    SECTION("A click without movement leaves model and undo stack untouched")
    {
        UITestHelper::simulateMousePress(&view, start);
        UITestHelper::simulateMouseRelease(&view, start);

        CHECK(scene.undoStack().count() == 0);
        CHECK(model->nodeData(nodeId, NodeRole::Position).value<QPointF>() == QPointF(0, 0));
    }

    SECTION("A click after a position was set from code does not emit nodeMoved")
    {
        QSignalSpy movedSpy(&scene, &BasicGraphicsScene::nodeMoved);

        model->setNodeData(nodeId, NodeRole::Position, QPointF(0, 0));
        UITestHelper::simulateMousePress(&view, start);
        UITestHelper::simulateMouseRelease(&view, start);

        CHECK(movedSpy.count() == 0);
    }

    SECTION("All selected nodes are written back")
    {
        NodeId const other = model->addNode("TestNode");
        model->setNodeData(other, NodeRole::Position, QPointF(300, 0));
        UITestHelper::waitForUI();

        auto *otherItem = scene.nodeGraphicsObject(other);
        REQUIRE(otherItem != nullptr);
        otherItem->setOpacity(1.0);
        item->setSelected(true);
        otherItem->setSelected(true);

        UITestHelper::simulateMouseDrag(&view, start, start + delta);

        CHECK(model->nodeData(nodeId, NodeRole::Position).value<QPointF>() == item->pos());
        CHECK(model->nodeData(other, NodeRole::Position).value<QPointF>() == otherItem->pos());
        CHECK(otherItem->pos() == QPointF(300, 0) + delta);
        CHECK(scene.undoStack().count() == 1);
    }
}
