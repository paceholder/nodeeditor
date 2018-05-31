#pragma once

#include <QtCore/QString>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QWidget>

#include "Export.hpp"

namespace QtNodes
{

class DataModelRegistry;

class NODE_EDITOR_PUBLIC ModelSelectionWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ModelSelectionWidget(DataModelRegistry& registry, QWidget* parent = Q_NULLPTR);

signals:
  void
  modelSelected(QString modelName);
};

}
