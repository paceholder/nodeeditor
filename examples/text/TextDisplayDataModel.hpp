#pragma once

#include "TextData.hpp"

#include <QtNodes/NodeDataModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <iostream>
#include <vector>

using QtNodes::ConnectionPolicy;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  TextDisplayDataModel();

  virtual
  ~TextDisplayDataModel() {}

public:

  QString
  caption() const override
  { return QString("Text Display"); }

  bool
  captionVisible() const override { return false; }

  static QString
  Name()
  { return QString("TextDisplayDataModel"); }

  QString
  name() const override
  { return TextDisplayDataModel::Name(); }

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex const port) override;

  ConnectionPolicy
  portInConnectionPolicy(PortIndex) const override
  {
    return ConnectionPolicy::Many;
  }

  void
  setInData(std::shared_ptr<NodeData> data, PortIndex const portIndex) override
  {
  }

  void
  setInData(std::shared_ptr<NodeData> data, int, const QUuid& connectionId)
  {
    auto textData = std::dynamic_pointer_cast<TextData>(data);

    auto it = std::find_if(inputTexts.begin(), inputTexts.end(),
        [this, &connectionId](const auto& e)
        {
            return e.first == connectionId;
        });
    if (textData)
    {
      if (it == inputTexts.end())
        inputTexts.emplace_back(connectionId, textData->text());
      else
        it->second = textData->text();
    }
    else
    {
      inputTexts.erase(it);
    }

    QStringList textList;
    for (auto&& entry : inputTexts) textList.push_back(entry.second);

    _label->setText(QStringLiteral("%1 inputs: %2")
        .arg(textList.size())
        .arg(textList.join(QStringLiteral(", "))));
    _label->adjustSize();
  }


  QWidget *
  embeddedWidget() override { return _label; }

private:

  QLabel * _label;
  std::vector<std::pair<QUuid, QString>> inputTexts;
};
