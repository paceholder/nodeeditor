#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeStyle>

#include <QtGui/QColor>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The class can potentially incapsulate any user data which need to
/// be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:
    NodeDataType type() const override { return NodeDataType{"MyNodeData", "My Node Data"}; }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MyDataModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    ~MyDataModel() = default;

public:
    QString caption() const override { return QString("My Data Model"); }

    QString name() const override { return QString("MyDataModel"); }

    bool labelEditable() const override { return true; }

public:
    QJsonObject save() const override
    {
        QJsonObject modelJson;

        modelJson["name"] = name();

        return modelJson;
    }

public:
    unsigned int nPorts(PortType const) const override { return 3; }

    NodeDataType dataType(PortType const, PortIndex const) const override
    {
        return MyNodeData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return std::make_shared<MyNodeData>(); }

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};

//------------------------------------------------------------------------------

/// Shows a per-node style. This node has the same ports as MyDataModel,
/// but its delegate model overrides the background and border colors of
/// the global style. It does this in the constructor, before the node is shown.
class MyGreenDataModel : public MyDataModel
{
    Q_OBJECT
public:
    MyGreenDataModel()
    {
        QtNodes::NodeStyle style = nodeStyle();
        style.setBackgroundColor(QColor(200, 235, 200));
        style.NormalBoundaryColor = QColor(60, 140, 60);
        setNodeStyle(style);
    }

    QString caption() const override { return QString("My Green Data Model"); }

    QString name() const override { return QString("MyGreenDataModel"); }
};
