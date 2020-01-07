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

constexpr size_t nodesPerGroup = 5;
constexpr size_t nGroups = 6;

TEST_CASE("Creating groups from node", "[node groups][namo]")
{
  class MockModel : public StubNodeDataModel
  {};

  auto setup = applicationSetup();

  FlowScene scene;
  FlowView  view(&scene);

  SECTION("Creating a group from a single node")
  {
    auto& node = scene.createNode(std::make_unique<MockModel>());
    auto nodeID = node.id();

    std::vector<Node*> nodeVec(1, &node);
    scene.createGroup(nodeVec);
    CHECK(scene.groups().size() == 1);
    CHECK(nodeID == scene.groups().begin()++->second->childNodes()[0]->id());
  }

  SECTION("Creating a group from multiple nodes")
  {

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

TEST_CASE("Deleting nodes and groups", "[node groups][namo]")
{
  class MockModel : public StubNodeDataModel
  {};

  auto setup = applicationSetup();

  FlowScene scene;
  FlowView  view(&scene);

  view.resize(640, 480);

  view.show();
  REQUIRE(QTest::qWaitForWindowExposed(&view));

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

  SECTION("Deleting nodes that belong to a group")
  {

    for (size_t i = 0; i < nGroups; i++)
    {
      for (size_t j = 0; j < nodesPerGroup - 1; j++)
      {
        QUuid currentNodeID = nodeIDs[i][j];
        auto& node = scene.nodes().at(currentNodeID);
        if (auto currentGroup = node->nodeGroup().lock(); currentGroup)
        {
          scene.removeNode(*node.get());
          auto ids = currentGroup->nodeIDs();
          auto nodeInGroupIt = std::find(ids.begin(), ids.end(), currentNodeID);
          // checks if the id was actually removed from the map
          CHECK(nodeInGroupIt == ids.end());
        }
      }
    }

  }

  SECTION("Removing a node from a group")
  {
    for (size_t i = 0; i < nGroups; i++)
    {
      for (size_t j = 0; j < nodesPerGroup - 1; j++)
      {
        QUuid currentNodeID = nodeIDs[i][j];
        auto& node = scene.nodes().at(currentNodeID);
        if (auto currentGroup = node->nodeGroup().lock(); currentGroup)
        {
          currentGroup->removeNode(node.get());
          auto ids = currentGroup->nodeIDs();
          auto nodeInGroupIt = std::find(ids.begin(), ids.end(), currentNodeID);

          // checks if the id was actually removed from the map
          CHECK(nodeInGroupIt == ids.end());

          // checks if the node still exists in the scene
          auto nodeInSceneIt = scene.nodes().find(currentNodeID);
          CHECK(nodeInSceneIt != scene.nodes().end());

          SECTION("...then deleting the node")
          {
            scene.removeNode(*node.get());
            ids = currentGroup->nodeIDs();
            auto nodeInSceneIt = scene.nodes().find(currentNodeID);
            // checks if the node was removed from the scene
            CHECK(nodeInSceneIt == scene.nodes().end());
          }

          SECTION("...then assigning the node to another group")
          {
            std::vector<Node*> nodeVec(1, node.get());
            auto new_group_weakptr = scene.createGroup(nodeVec);
            auto newGroup = new_group_weakptr.lock();
            auto newGroupIDs = newGroup->nodeIDs();
            auto nodeInGroupIt = std::find(newGroupIDs.begin(), newGroupIDs.end(), currentNodeID);

            // checks if the id was included in the new group
            CHECK(nodeInGroupIt != newGroupIDs.end());
          }

          SECTION("...then reassigning the node to the group")
          {
            scene.addNodeToGroup(node->id(), currentGroup->id());
            auto ids = currentGroup->nodeIDs();
            auto nodeInGroupIt = std::find(ids.begin(), ids.end(), currentNodeID);

            // checks if the id was actually included in the map
            CHECK(nodeInGroupIt != ids.end());
          }
        }
      }
    }
  }

  SECTION("Deleting a whole group")
  {
    // check if the group was deleted along with each node
    for (size_t i = 0; i < nGroups; i++)
    {
      scene.removeGroup(groupIDs[i]);
      auto groupIt = scene.groups().find(groupIDs[i]);
      CHECK(groupIt == scene.groups().end());

      for (size_t j = 0; j < nodesPerGroup; j++)
      {
        auto nodeIt = scene.nodes().find(nodeIDs[i][j]);
        CHECK(nodeIt == scene.nodes().end());
      }

    }
  }
}

TEST_CASE("Saving and loading groups", "[node groups][namo]")
{
  class MockModel : public StubNodeDataModel
  {};

  auto setup = applicationSetup();

  FlowScene scene;
  FlowView  view(&scene);

  SECTION("Saving a group file")
  {

    SECTION("Loading group from file") {}
  }
  SECTION("Saving a scene that includes a group") {}
  SECTION("Loading a scene that includes a group") {}
}
