#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include "PixmapData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageLoaderModel : public NodeDataModel
{
  Q_OBJECT

public:
  ImageLoaderModel();

  virtual
  ~ImageLoaderModel() {}

public:

  QString
  caption() const override
  { return QString("Image Source"); }

  QString
  name() const override { return QString("ImageLoaderModel"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<ImageLoaderModel>(); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", ImageLoaderModel::name());
  }

public:

  virtual QString
  modelName() const
  { return QString("Source Image"); }

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData>, int) override
  { }

  QWidget *
  embeddedWidget() override { return _label; }

  bool
  resizable() const override { return true; }

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

private:

  QLabel * _label;

  QPixmap _pixmap;
};
