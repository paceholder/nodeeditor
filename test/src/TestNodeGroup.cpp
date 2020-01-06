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

    std::vector< std::vector<QUuid> > nodeIDs(nGroups,
        std::vector<QUuid>(nodesPerGroup));
    std::vector<QUuid> groupIDs(nGroups);

    for (size_t i = 0; i < nGroups; i++)
    {
      std::vector<Node*> nodes;
      nodes.reserve(nodesPerGroup);
      for (size_t j = 0; j < nodesPerGroup; j++)
      {
        auto& node = scene.createNode(std::make_unique<MockModel>());
        nodes.push_back(&node);
        nodeIDs[i][j] = node.id();
      }
      auto group_weak = scene.createGroup(nodes);
      if (auto group = group_weak.lock(); group)
      {
        groupIDs[i] = group->id();
      }
    }

    SECTION("Finding groups in scene via ID")
    {
      for (auto& groupID : groupIDs)
      {
        auto it = scene.groups().find(groupID);
        CHECK(it != scene.groups().end());
      }
    }

    SECTION("Finding node IDs in groups map")
    {
      for (size_t i = 0; i < nGroups; i++)
      {
        auto group = scene.groups().at(groupIDs[i]);
        auto ids = group->nodeIDs();

        std::set<QUuid> sceneIdSet;
        sceneIdSet.insert(ids.begin(), ids.end());
        std::set<QUuid> localIdSet;
        localIdSet.insert(nodeIDs[i].begin(), nodeIDs[i].end());

        // checks if the IDs stored in the scene are the same
        // as the ones previously generated.
        CHECK(sceneIdSet == localIdSet);
      }
    }

    SECTION("Checking the group reference of each node")
    {
      for (auto& group : scene.groups())
      {
        auto id = group.first;
        for (auto& node : group.second->childNodes())
        {
          auto nodeGroup = node->nodeGroup().lock();
          CHECK(id == nodeGroup->id());
        }
      }
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
