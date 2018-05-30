#include <nodes/FlowScene>
#include <nodes/Node>

#include "ApplicationSetup.hpp"
#include "StubNodeDataModel.hpp"

#include <catch.hpp>

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;

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
