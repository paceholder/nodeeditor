#include <nodes/FlowScene>

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <nodes/Node>
#include <nodes/NodeDataModel>

#include <catch2/catch.hpp>

#include "ApplicationSetup.hpp"
#include "Stringify.hpp"
#include "StubNodeDataModel.hpp"

using QtNodes::Connection;
using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

TEST_CASE("FlowScene triggers connections created or deleted", "[gui]")
{
  struct MockDataModel : StubNodeDataModel
  {
    unsigned int nPorts(PortType) const override { return 1; }

    void
    inputConnectionCreated(Connection const&) override
    {
      inputCreatedCalledCount++;
    }

    void
    inputConnectionDeleted(Connection const&) override
    {
      inputDeletedCalledCount++;
    }

    void
    outputConnectionCreated(Connection const&) override
    {
      outputCreatedCalledCount++;
    }

    void
    outputConnectionDeleted(Connection const&) override
    {
      outputDeletedCalledCount++;
    }

    int inputCreatedCalledCount  = 0;
    int inputDeletedCalledCount  = 0;
    int outputCreatedCalledCount = 0;
    int outputDeletedCalledCount = 0;

    void
    resetCallCounts()
    {
      inputCreatedCalledCount  = 0;
      inputDeletedCalledCount  = 0;
      outputCreatedCalledCount = 0;
      outputDeletedCalledCount = 0;
    }
  };

  auto setup = applicationSetup();

  FlowScene scene;

  Node& fromNode      = scene.createNode(std::make_unique<MockDataModel>());
  Node& toNode        = scene.createNode(std::make_unique<MockDataModel>());
  Node& unrelatedNode = scene.createNode(std::make_unique<MockDataModel>());

  auto& fromNgo      = fromNode.nodeGraphicsObject();
  auto& toNgo        = toNode.nodeGraphicsObject();
  auto& unrelatedNgo = unrelatedNode.nodeGraphicsObject();

  fromNgo.setPos(0, 0);
  toNgo.setPos(200, 20);
  unrelatedNgo.setPos(-100, -100);

  auto& from      = dynamic_cast<MockDataModel&>(*fromNode.nodeDataModel());
  auto& to        = dynamic_cast<MockDataModel&>(*toNode.nodeDataModel());
  auto& unrelated = dynamic_cast<MockDataModel&>(*unrelatedNode.nodeDataModel());


  SECTION("creating half a connection (not finishing the connection)")
  {
    auto connection = scene.createConnection(PortType::Out, fromNode, 0);

    CHECK(from.inputCreatedCalledCount == 0);
    CHECK(from.outputCreatedCalledCount == 0);

    CHECK(to.inputCreatedCalledCount == 0);
    CHECK(to.outputCreatedCalledCount == 0);

    CHECK(unrelated.inputCreatedCalledCount == 0);
    CHECK(unrelated.outputCreatedCalledCount == 0);

    scene.deleteConnection(*connection);
  }

  struct Creation
  {
    std::string                                  name;
    std::function<std::shared_ptr<Connection>()> createConnection;
  };

  Creation sceneCreation{"scene.createConnection",
                         [&] { return scene.createConnection(toNode, 0, fromNode, 0); }};

  Creation partialCreation{"scene.createConnection-by partial", [&] {
                             auto connection = scene.createConnection(PortType::Out, fromNode, 0);
                             connection->setNodeToPort(toNode, PortType::In, 0);

                             return connection;
                           }};

  struct Deletion
  {
    std::string                                      name;
    std::function<void(Connection & connection)> deleteConnection;
  };

  Deletion sceneDeletion{"scene.deleteConnection",
                         [&](Connection & c) { scene.deleteConnection(c); }};

  Deletion partialDragDeletion{"scene-deleteConnectionByDraggingOff",
                               [&](Connection & c)
                               {
                                 PortIndex portIndex = c.getPortIndex(PortType::In);
                                 Node * node = c.getNode(PortType::In);
                                 node->nodeState().getEntries(PortType::In)[portIndex].clear();
                                 c.clearNode(PortType::In);
                               }};

  SECTION("creating a connection")
  {
    std::vector<Creation> cases({sceneCreation, partialCreation});

    for (Creation const& create : cases)
    {
      SECTION(create.name)
      {
        auto connection = create.createConnection();

        CHECK(from.inputCreatedCalledCount == 0);
        CHECK(from.outputCreatedCalledCount == 1);

        CHECK(to.inputCreatedCalledCount == 1);
        CHECK(to.outputCreatedCalledCount == 0);

        CHECK(unrelated.inputCreatedCalledCount == 0);
        CHECK(unrelated.outputCreatedCalledCount == 0);

        scene.deleteConnection(*connection);
      }
    }
  }

  SECTION("deleting a connection")
  {
    std::vector<Deletion> cases({sceneDeletion, partialDragDeletion});

    for (auto const& deletion : cases)
    {
      SECTION("deletion: " + deletion.name)
      {
        Connection & connection = *sceneCreation.createConnection();

        from.resetCallCounts();
        to.resetCallCounts();

        deletion.deleteConnection(connection);

        // Here the Connection reference becomes dangling

        CHECK(from.inputDeletedCalledCount == 0);
        CHECK(from.outputDeletedCalledCount == 1);

        CHECK(to.inputDeletedCalledCount == 1);
        CHECK(to.outputDeletedCalledCount == 0);

        CHECK(unrelated.inputDeletedCalledCount == 0);
        CHECK(unrelated.outputDeletedCalledCount == 0);
      }
    }
  }
}


TEST_CASE("FlowScene's DataModelRegistry outlives nodes and connections", "[asan][gui]")
{
  class MockDataModel : public StubNodeDataModel
  {
  public:
    MockDataModel(int* const& incrementOnDestruction)
      : incrementOnDestruction(incrementOnDestruction)
    {
    }

    ~MockDataModel()
    {
      (*incrementOnDestruction)++;
    }

    // The reference ensures that we point into the memory that would be free'd
    // if the DataModelRegistry doesn't outlive this node
    int* const& incrementOnDestruction;
  };

  struct MockDataModelCreator
  {
    MockDataModelCreator(int* shouldBeAliveWhenAssignedTo)
      : shouldBeAliveWhenAssignedTo(shouldBeAliveWhenAssignedTo)
    {
    }

    auto
    operator()() const
    {
      return std::make_unique<MockDataModel>(shouldBeAliveWhenAssignedTo);
    }

    int* shouldBeAliveWhenAssignedTo;
  };

  int modelsDestroyed = 0;

  // Introduce a new scope, so that modelsDestroyed will be alive even after the
  // FlowScene is destroyed.
  {
    auto setup = applicationSetup();

    auto registry = std::make_shared<DataModelRegistry>();
    registry->registerModel(MockDataModelCreator(&modelsDestroyed));

    modelsDestroyed = 0;

    FlowScene scene(std::move(registry));

    auto& node = scene.createNode(scene.registry().create("name"));

    // On destruction, if this `node` outlives its MockDataModelCreator,
    // (if it outlives the DataModelRegistry), then we trigger undefined
    // behavior through use-after-free. ASAN will catch that.
  }

  CHECK(modelsDestroyed == 1);
}
