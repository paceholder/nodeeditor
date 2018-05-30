#include "ModelSelectionWidget.hpp"

#include <utility>

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidgetAction>

#include <nodes/DataModelRegistry>

using QtNodes::DataModelRegistry;
using QtNodes::ModelSelectionWidget;


static auto const SkipText = QStringLiteral("skip me");


ModelSelectionWidget::
ModelSelectionWidget(DataModelRegistry& registry, QWidget* parent)
  : QWidget(parent)
{
  auto* layout = new QVBoxLayout(this);
  setLayout(layout);

  // Add filterbox to the context menu
  auto* filter = new QLineEdit(this);

  filter->setPlaceholderText(QStringLiteral("Filter"));
  filter->setClearButtonEnabled(true);

  auto* filterAction = new QWidgetAction(this);
  filterAction->setDefaultWidget(filter);

  addAction(filterAction);
  layout->addWidget(filter);

  // Add result treeview to the context menu
  auto* treeView = new QTreeWidget(this);
  treeView->header()->close();

  auto* treeViewAction = new QWidgetAction(this);
  treeViewAction->setDefaultWidget(treeView);

  addAction(treeViewAction);
  layout->addWidget(treeView);

  QMap<QString, QTreeWidgetItem*> topLevelItems;
  for (auto const& cat : registry.categoriesOrder())
  {
    auto item = new QTreeWidgetItem(treeView);
    item->setText(0, cat);
    item->setData(0, Qt::UserRole, SkipText);
    topLevelItems[cat] = item;
  }

  auto const& assocCategory = registry.registeredModelsCategoryAssociation();
  for (auto const& name : registry.registeredModelsOrder())
  {
    auto topLevelParent = topLevelItems[assocCategory.at(name)];
    auto item           = new QTreeWidgetItem(topLevelParent);
    item->setText(0, name);
    item->setData(0, Qt::UserRole, name);
  }

  treeView->expandAll();

  connect(treeView, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item) {
    QString modelName = item->data(0, Qt::UserRole).toString();

    if (modelName == SkipText)
    {
      return;
    }

    emit modelSelected(modelName);
  });

  // Setup filtering
  connect(filter, &QLineEdit::textChanged, [topLevelItems = std::move(topLevelItems)](const QString& text) {
    for (auto& topLvlItem : topLevelItems)
    {
      for (int i = 0; i < topLvlItem->childCount(); ++i)
      {
        auto child     = topLvlItem->child(i);
        auto modelName = child->data(0, Qt::UserRole).toString();
        if (modelName.contains(text, Qt::CaseInsensitive))
        {
          child->setHidden(false);
        }
        else
        {
          child->setHidden(true);
        }
      }
    }
  });

  // make sure the text box gets focus so the user doesn't have to click on it
  filter->setFocus();
}
