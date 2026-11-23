#include "ApplicationSetup.hpp"
#include "LockableModel.hpp"
#include "TestGraphModel.hpp"
#include "UITestHelper.hpp"

#include <QtNodes/internal/AbstractNodeGeometry.hpp>
#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/Definitions>

#include <catch2/catch.hpp>
#include <QUndoStack>

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortType;

TEST_CASE("Clicking a port of a locked node does not crash or start a connection",
          "[ui][lock]")
{
    auto app = applicationSetup();

    LockableModel<TestGraphModel> model;
    BasicGraphicsScene scene(model);
    GraphicsView view(&scene);
    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    NodeId const nodeId = model.addNode("TestNode");
    model.setNodeData(nodeId, NodeRole::Position, QPointF(0, 0));
    UITestHelper::waitForUI();

    auto *item = scene.nodeGraphicsObject(nodeId);
    REQUIRE(item != nullptr);

    // No style is loaded in tests, so the default opacity is 0 and Qt would
    // skip the node when hit-testing the simulated mouse events.
    item->setOpacity(1.0);

    model.setLocked(nodeId, true);

    QPointF const outPort = item->mapToScene(
        scene.nodeGeometry().portPosition(nodeId, PortType::Out, 0));

    // The click on a port without a connection, twice in a row, as in #396.
    for (int i = 0; i < 2; ++i) {
        UITestHelper::simulateMousePress(&view, outPort);
        UITestHelper::simulateMouseRelease(&view, outPort);
    }

    CHECK(model.allConnectionIds(nodeId).empty());
    CHECK(scene.undoStack().count() == 0);
    CHECK(item->pos() == QPointF(0, 0));
}
