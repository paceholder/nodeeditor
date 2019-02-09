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
}
