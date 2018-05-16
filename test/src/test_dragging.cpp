#include <nodes/Connection>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>

#include <catch.hpp>

#include <QtTest>
#include <QtWidgets/QApplication>

#include <iostream>

#include "GuiSetup.hpp"
#include "MockNodeDataModel.hpp"
#include "Stringify.hpp"

using QtNodes::Connection;
using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

TEST_CASE("Dragging node changes position", "[gui]")
{
  auto setup = guiSetup();

  FlowScene scene;
  FlowView  view(&scene);

  view.show();
  REQUIRE(QTest::qWaitForWindowExposed(&view));

  SECTION("just one node")
  {
    auto& node = scene.createNode(std::make_unique<MockNodeDataModel>());

    auto& ngo = node.nodeGraphicsObject();

    QPointF scPosBefore = ngo.pos();

    QPointF scClickPos = ngo.boundingRect().center();
    scClickPos         = QPointF(ngo.sceneTransform().map(scClickPos).toPoint());

    QPoint vwClickPos = view.mapFromScene(scClickPos);
    QPoint vwDestPos  = vwClickPos + QPoint(10, 20);

    QPointF scExpectedDelta = view.mapToScene(vwDestPos) - scClickPos;

    CAPTURE(scClickPos);
    CAPTURE(vwClickPos);
    CAPTURE(vwDestPos);
    CAPTURE(scExpectedDelta);

    QTest::mouseMove(view.windowHandle(), vwClickPos);
    QTest::mousePress(view.windowHandle(), Qt::LeftButton, Qt::NoModifier, vwClickPos);
    QTest::mouseMove(view.windowHandle(), vwDestPos);

    QPointF scDelta            = ngo.pos() - scPosBefore;
    QPoint  roundDelta         = scDelta.toPoint();
    QPoint  roundExpectedDelta = scExpectedDelta.toPoint();

    CHECK(roundDelta == roundExpectedDelta);
  }
}

TEST_CASE("Issue 169", "[gui][regression]")
{
  class MockModel : public MockNodeDataModel
  {
  public:
    unsigned int nPorts(PortType) const override { return 2; }
  };

  auto setup = guiSetup();

  FlowScene scene;
  FlowView  view(&scene);

  view.resize(640, 480);

  view.show();
  REQUIRE(QTest::qWaitForWindowExposed(&view));

  Node& from  = scene.createNode(std::make_unique<MockModel>());
  Node& to    = scene.createNode(std::make_unique<MockModel>());
  Node& third = scene.createNode(std::make_unique<MockModel>());

  auto& fromNgo  = from.nodeGraphicsObject();
  auto& toNgo    = to.nodeGraphicsObject();
  auto& thirdNgo = third.nodeGraphicsObject();

  fromNgo.setPos(QPointF(50, 50));

  qreal offset  = fromNgo.boundingRect().right();
  qreal padding = 20;
  thirdNgo.setPos(QPointF(50 + offset + padding, 200));

  toNgo.setPos(QPointF(50 + 2 * offset + 2 * padding, 50));

  auto        connectionPtr = scene.createConnection(to, 0, from, 0);
  Connection& connection    = *connectionPtr;
  auto&       cgo           = connection.getConnectionGraphicsObject();

  scene.createConnection(third, 0, from, 0);
  scene.createConnection(to, 1, third, 0);

  auto const getPos = [&](auto& graphicsObject) -> QPoint {
    auto const& transform = graphicsObject.sceneTransform();
    QPointF     center    = graphicsObject.boundingRect().center();

    return view.mapFromScene(transform.map(center));
  };

  QPoint oldConnectionPos = getPos(cgo);
  QPoint oldThirdPos      = getPos(thirdNgo);
  CAPTURE(oldConnectionPos);
  CAPTURE(oldThirdPos);

  cgo.setSelected(true);
  thirdNgo.setSelected(true);

  {
    INFO("Failed to select the connection and node (bug in test)");
    REQUIRE(scene.selectedItems().size() == 2);
  }

  auto const mouseDrag = [&](QPoint from, QPoint to) {
    QTest::mousePress(view.windowHandle(), Qt::LeftButton, Qt::NoModifier, from, 1000);
    QTest::mouseRelease(view.windowHandle(), Qt::LeftButton, Qt::NoModifier, to, 1000);
  };
  QPoint newThirdPos = QPoint(oldThirdPos.x(), oldThirdPos.y() - 10);
  mouseDrag(oldThirdPos, newThirdPos);

  {
    INFO("Connection moved, visually disconnecting it from the ports (as reported in #169)");
    CHECK(oldConnectionPos == getPos(cgo));
  }
  {
    INFO("Node didn't move when it should have (bug in test)");
    CHECK(newThirdPos == getPos(thirdNgo));
    CHECK(oldThirdPos != getPos(thirdNgo));
  }
}
