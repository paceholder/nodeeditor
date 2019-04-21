#include "ModelSelectionWidget.hpp"

#include <nodes/DataModelRegistry>

#include <QtCore/QObject>
#include <QtTest>
#include <QtWidgets/QTreeWidget>

#include <catch2/catch.hpp>

#include "ApplicationSetup.hpp"
#include "Stringify.hpp"
#include "StubNodeDataModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::ModelSelectionWidget;

#include <iostream>
#include <typeinfo>

TEST_CASE("ModelSelectionWidget models order (within a category)", "[gui]")
{
  class StubModel : public StubNodeDataModel
  {
  public:
    StubModel(QString modelName)
      : modelName(modelName)
    {
    }

    QString
    name() const override
    {
      return modelName;
    }

    QString modelName;
  };

  auto setup = applicationSetup();

  DataModelRegistry registry;
  registry.registerModel<StubModel>([] {
    return std::make_unique<StubModel>("a");
  });
  registry.registerModel<StubModel>([] {
    return std::make_unique<StubModel>("c");
  });
  registry.registerModel<StubModel>([] {
    return std::make_unique<StubModel>("b");
  });

  auto widget = std::make_unique<ModelSelectionWidget>(registry);

  auto tree = widget->findChild<QTreeWidget*>();

  {
    INFO("Test out of date. ModelSelectionWidget doesn't use QTreeWidget");
    REQUIRE(tree != nullptr);
  }

  REQUIRE(tree->topLevelItemCount() == 1);

  QTreeWidgetItem* mainCategory = tree->topLevelItem(0);

  REQUIRE(mainCategory->childCount() == 3);
  CHECK(mainCategory->child(0)->data(0, Qt::UserRole).toString().toStdString() == "a");
  CHECK(mainCategory->child(1)->data(0, Qt::UserRole).toString().toStdString() == "c");
  CHECK(mainCategory->child(2)->data(0, Qt::UserRole).toString().toStdString() == "b");
}

#include <nodes/FlowScene>
#include <nodes/FlowView>

TEST_CASE("ModelSelectionWidget category order", "[gui]")
{
  auto setup = applicationSetup();

  DataModelRegistry registry;

  registry.registerModel<StubNodeDataModel>("Category A", [] {
    auto result = std::make_unique<StubNodeDataModel>();

    result->name("A");

    return result;
  });

  registry.registerModel<StubNodeDataModel>("Category C", [] {
    auto result = std::make_unique<StubNodeDataModel>();

    result->name("C");

    return result;
  });

  registry.registerModel<StubNodeDataModel>("Category B", [] {
    auto result = std::make_unique<StubNodeDataModel>();

    result->name("B");

    return result;
  });

  SECTION("unsorted categories")
  {
    auto widget = std::make_unique<ModelSelectionWidget>(registry);

    auto tree = widget->findChild<QTreeWidget*>();

    {
      INFO("Test out of date. ModelSelectionWidget doesn't use QTreeWidget");
      REQUIRE(tree != nullptr);
    }

    REQUIRE(tree->topLevelItemCount() == 3);

    CHECK(tree->topLevelItem(0)->text(0).toStdString() == "Category A");
    CHECK(tree->topLevelItem(1)->text(0).toStdString() == "Category C");
    CHECK(tree->topLevelItem(2)->text(0).toStdString() == "Category B");
  }
  SECTION("sorted categories")
  {
    registry.sortCategories();

    auto widget = std::make_unique<ModelSelectionWidget>(registry);

    auto tree = widget->findChild<QTreeWidget*>();

    {
      INFO("Test out of date. ModelSelectionWidget doesn't use QTreeWidget");
      REQUIRE(tree != nullptr);
    }

    REQUIRE(tree->topLevelItemCount() == 3);

    CHECK(tree->topLevelItem(0)->text(0).toStdString() == "Category A");
    CHECK(tree->topLevelItem(1)->text(0).toStdString() == "Category B");
    CHECK(tree->topLevelItem(2)->text(0).toStdString() == "Category C");
  }
}
