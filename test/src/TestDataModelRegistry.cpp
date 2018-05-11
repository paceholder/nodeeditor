#include <nodes/DataModelRegistry>

#include <catch2/catch.hpp>

#include "StubNodeDataModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace
{
class StubModelStaticName : public StubNodeDataModel
{
public:
  static QString
  Name()
  {
    return "Name";
  }
};
}

TEST_CASE("DataModelRegistry::registerModel", "[interface]")
{
  DataModelRegistry registry;

  SECTION("stub model")
  {
    registry.registerModel<StubNodeDataModel>();
    auto model = registry.create("name");

    CHECK(model->name() == "name");
  }
  SECTION("stub model with static name")
  {
    registry.registerModel<StubModelStaticName>();
    auto model = registry.create("Name");

    CHECK(model->name() == "name");
  }
  SECTION("From model creator function")
  {
    SECTION("non-static name()")
    {
      registry.registerModel([] {
        return std::make_unique<StubNodeDataModel>();
      });

      auto model = registry.create("name");

      REQUIRE(model != nullptr);
      CHECK(model->name() == "name");
      CHECK(dynamic_cast<StubNodeDataModel*>(model.get()));
    }
    SECTION("static Name()")
    {
      registry.registerModel([] {
        return std::make_unique<StubModelStaticName>();
      });

      auto model = registry.create("Name");

      REQUIRE(model != nullptr);
      CHECK(model->name() == "name");
      CHECK(dynamic_cast<StubModelStaticName*>(model.get()));
    }
  }
}
