#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>
#include <nodes/NodeDataModel>

#include <catch2/catch.hpp>

#include <QtTest>

#include "ApplicationSetup.hpp"
#include "StubNodeDataModel.hpp"

using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::Node;
using QtNodes::NodeDataModel;
using QtNodes::NodeGraphicsObject;
using QtNodes::PortType;

TEST_CASE("NodeDataModel::portOutConnectionPolicy(...) isn't called for input "
          "connections (issue #127)",
          "[gui]")
{
  class MockModel : public StubNodeDataModel
  {
  public:
    unsigned int nPorts(PortType) const override { return 1; }

    NodeDataModel::ConnectionPolicy
    portOutConnectionPolicy(int index) const override
    {
      portOutConnectionPolicyCalledCount++;
      return NodeDataModel::ConnectionPolicy::One;
    }

    mutable int portOutConnectionPolicyCalledCount = 0;
  };

  auto setup = applicationSetup();

  FlowScene scene;
  FlowView  view(&scene);

  // Ensure we have enough size to contain the node
  view.resize(640, 480);

  view.show();
  REQUIRE(QTest::qWaitForWindowExposed(&view));

  auto& node  = scene.createNode(std::make_unique<MockModel>());
  auto& model = dynamic_cast<MockModel&>(*node.nodeDataModel());
  auto& ngo   = node.nodeGraphicsObject();
  auto& ngeom = node.nodeGeometry();

  // Move the node to somewhere in the middle of the screen
  ngo.setPos(QPointF(50, 50));

  // Compute the on-screen position of the input port
  QPointF scInPortPos = ngeom.portScenePosition(0, PortType::In, ngo.sceneTransform());
  QPoint  vwInPortPos = view.mapFromScene(scInPortPos);

  // Create a partial connection by clicking on the input port of the node
  QTest::mousePress(view.windowHandle(), Qt::LeftButton, Qt::NoModifier, vwInPortPos);

  CHECK(model.portOutConnectionPolicyCalledCount == 0);
}
