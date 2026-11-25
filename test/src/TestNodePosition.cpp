#include "ApplicationSetup.hpp"
#include "UITestHelper.hpp"

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/internal/GroupGraphicsObject.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/NodeGroup.hpp>

#include <catch2/catch.hpp>
#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>
#include <QSignalSpy>

namespace {
class PositionTestNode : public QtNodes::NodeDelegateModel
{
public:
    QString name() const override { return "PositionTestNode"; }
    QString caption() const override { return "Position Test"; }
    unsigned int nPorts(QtNodes::PortType) const override { return 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override
    {
        return {};
    }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const) override {}
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const) override
    {
        return nullptr;
    }
    QWidget *embeddedWidget() override { return nullptr; }
};
} // namespace

TEST_CASE("Graphical node position changes reach the model", "[position]")
{
    auto app = applicationSetup();
    qRegisterMetaType<QtNodes::NodeId>("NodeId");
    auto registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    registry->registerModel<PositionTestNode>("PositionTestNode");
    QtNodes::DataFlowGraphModel model(registry);
    auto const nodeId = model.addNode("PositionTestNode");
    QPointF const initial(100, 150);
    QPointF const destination(160, 190);
    model.setNodeData(nodeId, QtNodes::NodeRole::Position, initial);
    QtNodes::BasicGraphicsScene scene(model);
    auto *node = scene.nodeGraphicsObject(nodeId);
    REQUIRE(node != nullptr);
    QSignalSpy updates(&model, &QtNodes::AbstractGraphModel::nodePositionUpdated);

    SECTION("Graphical moves update and serialize the position")
    {
        node->setPos(destination);
        CHECK(model.nodeData(nodeId, QtNodes::NodeRole::Position).value<QPointF>() == destination);
        CHECK(updates.count() == 1);
        QJsonObject const position = model.saveNode(nodeId)["position"].toObject();
        CHECK(position["x"].toDouble() == destination.x());
        CHECK(position["y"].toDouble() == destination.y());
        QtNodes::DataFlowGraphModel restored(registry);
        restored.load(model.save());
        CHECK(restored.nodeData(nodeId, QtNodes::NodeRole::Position).value<QPointF>()
              == destination);
    }

    SECTION("Dragging updates the model and reports the completed move")
    {
        QtNodes::GraphicsView view(&scene);
        view.resize(800, 600);
        view.show();
        REQUIRE(QTest::qWaitForWindowExposed(&view));
        UITestHelper::waitForUI();
        QSignalSpy moved(&scene, &QtNodes::BasicGraphicsScene::nodeMoved);
        QPointF const start = node->mapToScene(node->boundingRect().center());
        UITestHelper::simulateMouseDrag(&view, start, start + QPointF(40, 30));
        CHECK(node->pos() != initial);
        CHECK(model.nodeData(nodeId, QtNodes::NodeRole::Position).value<QPointF>() == node->pos());
        REQUIRE(moved.count() == 1);
        CHECK(moved.at(0).at(1).toPointF() == node->pos());
    }

    SECTION("Moving a group updates every member position")
    {
        auto const secondId = model.addNode("PositionTestNode");
        QPointF const secondInitial(300, 150);
        model.setNodeData(secondId, QtNodes::NodeRole::Position, secondInitial);
        auto *second = scene.nodeGraphicsObject(secondId);
        REQUIRE(second != nullptr);
        std::vector<QtNodes::NodeGraphicsObject *> members{node, second};
        auto group = scene.createGroup(members, "Position group").lock();
        REQUIRE(group);
        updates.clear();
        QPointF const offset(20, 30);
        group->groupGraphicsObject().moveNodes(offset);
        CHECK(model.nodeData(nodeId, QtNodes::NodeRole::Position).value<QPointF>()
              == initial + offset);
        CHECK(model.nodeData(secondId, QtNodes::NodeRole::Position).value<QPointF>()
              == secondInitial + offset);
        CHECK(updates.count() == 2);
    }

    SECTION("Model updates do not emit duplicate position changes")
    {
        model.setNodeData(nodeId, QtNodes::NodeRole::Position, destination);
        CHECK(node->pos() == destination);
        CHECK(updates.count() == 1);
    }

    SECTION("Unchanged graphical positions do not notify the model")
    {
        node->setPos(initial);
        CHECK(updates.count() == 0);
    }

    SECTION("Mouse release reports a completed graphical move")
    {
        QSignalSpy moved(&scene, &QtNodes::BasicGraphicsScene::nodeMoved);
        node->setSelected(true);
        node->setPos(destination);
        QGraphicsSceneMouseEvent release(QEvent::GraphicsSceneMouseRelease);
        release.setButton(Qt::LeftButton);
        release.setPos(node->boundingRect().center());
        scene.sendEvent(node, &release);
        REQUIRE(moved.count() == 1);
        CHECK(moved.at(0).at(1).toPointF() == destination);
    }
}
