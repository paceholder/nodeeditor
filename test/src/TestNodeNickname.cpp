#include "ApplicationSetup.hpp"

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/Definitions>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include <catch2/catch.hpp>

#include <QSignalSpy>

using QtNodes::DataFlowGraphModel;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace {

// Creates a test nickname node, implementing the NodeDelegateModel interface
// As a minimal node, it has no inputs or outputs, no port data type, and no embedded widget
class NicknameNode : public NodeDelegateModel
{
    public:
        QString name() const override { return QStringLiteral("NicknameNode"); }
        QString caption() const override { return QStringLiteral("Test Node"); }
        QString label() const override { return QStringLiteral("Default nickname"); }
        bool labelVisible() const override { return false; }
        bool labelEditable() const override { return true; }

        unsigned int nPorts(PortType) const override { return 0; }
        NodeDataType dataType(PortType, PortIndex) const override { return {}; }
        void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
        std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }
        QWidget *embeddedWidget() override { return nullptr; }
};

std::shared_ptr<NodeDelegateModelRegistry> makeRegistry()
{
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<NicknameNode>();
    return registry;
}

} // namespace

TEST_CASE("Nickname can be read and changed", "[nickname]")
{
    auto app = applicationSetup();
    DataFlowGraphModel model(makeRegistry());

    NodeId const nodeId = model.addNode(QStringLiteral("NicknameNode"));

    // Check the default nickname, initial visibility, and editability
    CHECK(model.nodeData(nodeId, NodeRole::Label).toString() == QStringLiteral("Default nickname"));
    CHECK_FALSE(model.nodeData(nodeId, NodeRole::LabelVisible).toBool());
    CHECK(model.nodeData(nodeId, NodeRole::LabelEditable).toBool());

    QSignalSpy nodeUpdatedSpy(&model, &DataFlowGraphModel::nodeUpdated);

    // Check if the nickname can be changed and the visibility can be setted
    // Also check if the signals for these changes have been emitted
    CHECK(model.setNodeData(nodeId, NodeRole::Label, QStringLiteral("New nickname")));
    CHECK(model.setNodeData(nodeId, NodeRole::LabelVisible, true));
    CHECK(nodeUpdatedSpy.count() == 2);
}

TEST_CASE("Nickname is saved and loaded", "[nickname]")
{
    auto app = applicationSetup();
    auto registry = makeRegistry();
    DataFlowGraphModel model(registry);

    NodeId const nodeId = model.addNode(QStringLiteral("NicknameNode"));

    // Change the nodes visibility and nickname
    model.setNodeData(nodeId, NodeRole::Label, QStringLiteral("Saved nickname"));
    model.setNodeData(nodeId, NodeRole::LabelVisible, true);

    // Serialize the node and load it into a new model
    QJsonObject const nodeJson = model.saveNode(nodeId);
    DataFlowGraphModel restoredModel(registry);
    restoredModel.loadNode(nodeJson);

    // Check if the nickname and its visibility have been restored correctly
    CHECK(restoredModel.nodeData(nodeId, NodeRole::Label).toString()
          == QStringLiteral("Saved nickname"));
    CHECK(restoredModel.nodeData(nodeId, NodeRole::LabelVisible).toBool());
}
