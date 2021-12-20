#pragma once

#include <utility>

#include <nodes/NodeDataModel>

class StubNodeDataModel : public QtNodes::NodeDataModel
{
public:
  QString
  name() const override
  {
    return _name;
  }

  QString
  nickname() const override
  {
    return QString();
  }

  QString
  caption() const override
  {
    return _caption;
  }

  QString
  progressValue() const override
  {
    return QString();
  }

  unsigned int nPorts(QtNodes::PortType) const override
  {
    return 0;
  }

  QWidget*
  embeddedWidget() override
  {
    return nullptr;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override
  {
    return QtNodes::NodeDataType();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override
  {
    return nullptr;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override
  {
  }

  void
  name(QString name)
  {
    _name = std::move(name);
  }

  void
  caption(QString caption)
  {
    _caption = std::move(caption);
  }

private:
  QString _name    = "name";
  QString _caption = "caption";
};
