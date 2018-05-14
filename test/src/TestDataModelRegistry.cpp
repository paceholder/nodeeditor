#include <nodes/DataModelRegistry>

#include <catch.hpp>

using QtNodes::DataModelRegistry;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace
{
struct StubModel : public NodeDataModel
{
  QString
  caption() const override
  {
    return "caption";
  }

  QString
  name() const override
  {
    return "name";
  }

  unsigned int
  nPorts(PortType type) const override
  {
    return 0;
  }

  NodeDataType
  dataType(PortType type, PortIndex index) const override
  {
    return {};
  }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex index) override
  {
  }

  std::shared_ptr<NodeData>
  outData(PortIndex index) override
  {
    return nullptr;
  }

  QWidget*
  embeddedWidget() override
  {
    return nullptr;
  }
};

struct StubModelStaticName : public StubModel
{
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
    registry.registerModel<StubModel>();
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
