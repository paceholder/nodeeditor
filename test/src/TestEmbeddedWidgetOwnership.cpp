#include "ApplicationSetup.hpp"

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/Definitions>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include <catch2/catch.hpp>

#include <QLineEdit>
#include <QPointer>
#include <QUndoStack>

#include <memory>
#include <vector>

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace {

// Every widget made by EagerWidgetModel, in creation order.
std::vector<QPointer<QWidget>> &createdWidgets()
{
    static std::vector<QPointer<QWidget>> widgets;
    return widgets;
}

// A model that creates its widget in the constructor. This is the pattern
// that leaked, because the registry makes a temporary model.
// The model keeps the widget in a QPointer and deletes it in the destructor.
class EagerWidgetModel : public NodeDelegateModel
{
public:
    EagerWidgetModel()
        : _widget(new QLineEdit())
    {
        createdWidgets().push_back(_widget.data());
    }

    ~EagerWidgetModel() override { delete _widget; }

    QString name() const override { return QStringLiteral("EagerWidgetModel"); }
    QString caption() const override { return QStringLiteral("Eager"); }

    unsigned int nPorts(PortType) const override { return 0; }
    NodeDataType dataType(PortType, PortIndex) const override { return {}; }
    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
    std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }

    QWidget *embeddedWidget() override { return _widget; }

private:
    QPointer<QLineEdit> _widget;
};

std::shared_ptr<NodeDelegateModelRegistry> makeRegistry()
{
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<EagerWidgetModel>();
    return registry;
}

int aliveWidgets()
{
    int count = 0;
    for (auto const &w : createdWidgets())
        if (w)
            ++count;
    return count;
}

} // namespace

TEST_CASE("A widget made by the temporary registry model is deleted with that model",
          "[widget-ownership]")
{
    auto app = applicationSetup();
    createdWidgets().clear();

    auto registry = makeRegistry();

    // Registration made exactly one temporary model, and it is already gone.
    REQUIRE(createdWidgets().size() == 1);
    CHECK(aliveWidgets() == 0);
}

TEST_CASE("A model that deletes its widget in the destructor is safe in a scene",
          "[widget-ownership]")
{
    auto app = applicationSetup();
    createdWidgets().clear();

    auto registry = makeRegistry();
    createdWidgets().clear();

    SECTION("Deleting the node frees its widget once")
    {
        DataFlowGraphModel model(registry);
        DataFlowGraphicsScene scene(model);

        NodeId const id = model.addNode(QStringLiteral("EagerWidgetModel"));
        REQUIRE(createdWidgets().size() == 1);
        CHECK(aliveWidgets() == 1);

        CHECK(model.deleteNode(id));
        CHECK(aliveWidgets() == 0);
    }

    SECTION("Undo of a delete makes a new widget and the old one stays freed")
    {
        DataFlowGraphModel model(registry);
        DataFlowGraphicsScene scene(model);

        NodeId const id = model.addNode(QStringLiteral("EagerWidgetModel"));
        scene.nodeGraphicsObject(id)->setSelected(true);

        scene.onDeleteSelectedObjects();
        REQUIRE(createdWidgets().size() == 1);
        CHECK(aliveWidgets() == 0);

        scene.undoStack().undo();
        REQUIRE(createdWidgets().size() == 2);
        CHECK(aliveWidgets() == 1);

        scene.undoStack().redo();
        CHECK(aliveWidgets() == 0);
    }

    SECTION("Destroying the scene before the model frees the widget once")
    {
        DataFlowGraphModel model(registry);
        {
            DataFlowGraphicsScene scene(model);
            model.addNode(QStringLiteral("EagerWidgetModel"));
            CHECK(aliveWidgets() == 1);
        }
        // The scene is gone. The proxy widget deleted the widget with it.
        // The model destructor must not delete it a second time.
    }

    CHECK(aliveWidgets() == 0);
}
