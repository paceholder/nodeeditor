#include "ModelSelectionWidget.hpp"

#include <nodes/DataModelRegistry>

#include <QtCore/QObject>
#include <QtTest>
#include <QtWidgets/QTreeWidget>

#include <catch.hpp>

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
