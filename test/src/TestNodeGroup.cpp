#include <QtTest>
#include <catch2/catch.hpp>

#include <nodes/NodeGroup>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>

#include "ApplicationSetup.hpp"
#include "StubNodeDataModel.hpp"

using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::Node;

TEST_CASE("Creating groups from node", "[groups], [namo]")
{
  class MockModel : public StubNodeDataModel
  {};

  auto setup = applicationSetup();

  FlowScene scene;
  FlowView  view(&scene);

  // Ensure we have enough size to contain the node
  view.resize(640, 480);

  view.show();
  REQUIRE(QTest::qWaitForWindowExposed(&view));

  SECTION("Creating a group from a single node")
  {
    // check number of groups in scene, node affiliation to group, IDs
    auto& node = scene.createNode(std::make_unique<MockModel>());
    auto nodeID = node.id();

    std::vector<Node*> nodeVec(1, &node);
    scene.createGroup(nodeVec);
    CHECK(scene.groups().size() == 1);
    CHECK(nodeID == scene.groups().begin()++->second->childNodes()[0]->id());
  }

  SECTION("Creating a group from multiple nodes")
  {
    constexpr size_t nodesPerGroup = 5;
    constexpr size_t nGroups = 2;
    for (size_t i = 0; i < nGroups; i++)
    {
      std::vector<Node*> nodes;
      nodes.reserve(nodesPerGroup);
      for (size_t j = 0; j < nodesPerGroup; j++)
      {
        auto& node = scene.createNode(std::make_unique<MockModel>());
        nodes.push_back(&node);
      }
      scene.createGroup(nodes);
    }
  }
}

TEST_CASE("Deleting nodes and groups", "[groups], [namo]")
{
  SECTION("Deleting a node associated with a group") {}

  SECTION("Removing a node from a group")
  {
    SECTION("Then deleting the node") {}

    SECTION("Then assigning the node to another group") {}
  }

  SECTION("Deleting a whole group") {}
}

TEST_CASE("Saving and loading groups", "[groups], [namo]")
{
  SECTION("Saving a group file")
  {

    SECTION("Loading group from file") {}
  }
  SECTION("Saving a scene that includes a group") {}
  SECTION("Loading a scene that includes a group") {}
}
