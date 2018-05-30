#pragma once

#include <QtCore/QString>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QWidget>

namespace QtNodes
{

class DataModelRegistry;

class ModelSelectionWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ModelSelectionWidget(DataModelRegistry& registry, QWidget* parent = Q_NULLPTR);

signals:
  void
  modelSelected(QString modelName);
};

}
