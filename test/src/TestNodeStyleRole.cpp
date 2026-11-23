#include "ApplicationSetup.hpp"
#include "TestDelegateModels.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/NodeStyle.hpp>
#include <QtNodes/internal/StyleCollection.hpp>

#include <QJsonDocument>
#include <QSignalSpy>

using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::NodeStyle;
using QtNodes::PortIndex;
using QtNodes::PortType;
using QtNodes::StyleCollection;

namespace {

/// Delegate that sets its own style in the constructor.
class StyledModel : public StubDelegateModel
{
public:
    StyledModel()
        : StubDelegateModel("Styled", "Styled")
    {
        NodeStyle style = nodeStyle();
        style.PenWidth = 7.0f;
        style.setBackgroundColor(QColor(10, 20, 30));
        setNodeStyle(style);
    }

    void changeStyle(QColor const &color)
    {
        NodeStyle style = nodeStyle();
        style.setBackgroundColor(color);
        setNodeStyle(style);
    }

    void requestUpdate() { Q_EMIT requestNodeUpdate(); }
};

/// Delegate that never sets its style. It keeps the global style copied at construction.
std::unique_ptr<NodeDelegateModel> createPlainModel()
{
    return std::make_unique<StubDelegateModel>("Plain", "Plain");
}

NodeStyle styleOf(DataFlowGraphModel &model, NodeId id)
{
    QVariant const v = model.nodeData(id, NodeRole::Style);
    REQUIRE(v.isValid());
    return NodeStyle(QJsonDocument::fromVariant(v).object());
}

std::shared_ptr<NodeDelegateModelRegistry> makeRegistry()
{
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<StyledModel>("Test");
    return registry;
}

/// Restores the global node style when the test ends.
struct GlobalStyleGuard
{
    NodeStyle saved{StyleCollection::nodeStyle()};
    ~GlobalStyleGuard() { StyleCollection::setNodeStyle(saved); }
};

} // namespace

TEST_CASE("The style role returns the style set on the node's delegate model, not the global one",
          "[core][style]")
{
    GlobalStyleGuard guard;
    DataFlowGraphModel model(makeRegistry());
    NodeId const id = model.addNode("Styled");
    REQUIRE(model.nodeExists(id));

    NodeStyle const global = StyleCollection::nodeStyle();
    REQUIRE(global.PenWidth != 7.0f);
    REQUIRE(global.GradientColor0 != QColor(10, 20, 30));

    NodeStyle const style = styleOf(model, id);
    CHECK(style.PenWidth == 7.0f);
    CHECK(style.GradientColor0 == QColor(10, 20, 30));
    CHECK(style.GradientColor3 == QColor(10, 20, 30));
}

TEST_CASE("Two nodes of the same type keep their own styles", "[core][style]")
{
    GlobalStyleGuard guard;
    DataFlowGraphModel model(makeRegistry());
    NodeId const a = model.addNode("Styled");
    NodeId const b = model.addNode("Styled");

    model.delegateModel<StyledModel>(a)->changeStyle(QColor(200, 0, 0));
    model.delegateModel<StyledModel>(b)->changeStyle(QColor(0, 200, 0));

    CHECK(styleOf(model, a).GradientColor0 == QColor(200, 0, 0));
    CHECK(styleOf(model, b).GradientColor0 == QColor(0, 200, 0));

    model.delegateModel<StyledModel>(a)->changeStyle(QColor(0, 0, 200));
    CHECK(styleOf(model, a).GradientColor0 == QColor(0, 0, 200));
    CHECK(styleOf(model, b).GradientColor0 == QColor(0, 200, 0));
}

TEST_CASE("A node that never sets a style keeps the global style from the time it was created",
          "[core][style]")
{
    GlobalStyleGuard guard;
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel(createPlainModel, "Test");
    DataFlowGraphModel model(registry);

    NodeStyle changed = StyleCollection::nodeStyle();
    changed.PenWidth = 3.0f;
    StyleCollection::setNodeStyle(changed);
    NodeId const early = model.addNode("Plain");

    changed.PenWidth = 5.0f;
    StyleCollection::setNodeStyle(changed);
    NodeId const late = model.addNode("Plain");

    CHECK(styleOf(model, early).PenWidth == 3.0f);
    CHECK(styleOf(model, late).PenWidth == 5.0f);
}

TEST_CASE("A node update request after a style change makes the model emit nodeUpdated",
          "[graphics][style]")
{
    auto app = applicationSetup();
    GlobalStyleGuard guard;
    DataFlowGraphModel model(makeRegistry());
    NodeId const id = model.addNode("Styled");
    QtNodes::BasicGraphicsScene scene(model);
    REQUIRE(scene.nodeGraphicsObject(id) != nullptr);

    QSignalSpy spy(&model, &DataFlowGraphModel::nodeUpdated);
    auto *delegate = model.delegateModel<StyledModel>(id);

    delegate->changeStyle(QColor(1, 2, 3));
    CHECK(spy.count() == 0);

    delegate->requestUpdate();
    REQUIRE(spy.count() == 1);
    CHECK(spy.first().first().value<NodeId>() == id);
    CHECK(styleOf(model, id).GradientColor0 == QColor(1, 2, 3));
}
