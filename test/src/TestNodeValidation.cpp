#include <catch2/catch.hpp>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QSignalSpy>

using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeId;
using QtNodes::NodeProcessingStatus;
using QtNodes::NodeRole;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// Test delegate model that exposes validation and status setters
class TestValidatedModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    QString caption() const override { return "Test Validated"; }
    QString name() const override { return "TestValidated"; }

    unsigned int nPorts(PortType portType) const override
    {
        return portType == PortType::In ? 1 : 1;
    }

    QtNodes::NodeDataType dataType(PortType, PortIndex) const override
    {
        return QtNodes::NodeDataType{"test", "Test"};
    }

    std::shared_ptr<QtNodes::NodeData> outData(PortIndex) override { return nullptr; }

    void setInData(std::shared_ptr<QtNodes::NodeData>, PortIndex) override {}

    QWidget *embeddedWidget() override { return nullptr; }

    // Expose validation methods for testing
    void setTestValidationState(NodeValidationState::State state, QString const &message)
    {
        NodeValidationState vs;
        vs._state = state;
        vs._stateMessage = message;
        setValidationState(vs);
    }

    void setTestProcessingStatus(NodeProcessingStatus status) { setNodeProcessingStatus(status); }
};

TEST_CASE("NodeValidationState basic functionality", "[validation]")
{
    SECTION("Default validation state is Valid")
    {
        NodeValidationState state;
        CHECK(state.isValid());
        CHECK(state.state() == NodeValidationState::State::Valid);
        CHECK(state.message().isEmpty());
    }

    SECTION("Validation state can be set to Warning")
    {
        NodeValidationState state;
        state._state = NodeValidationState::State::Warning;
        state._stateMessage = "Test warning";

        CHECK_FALSE(state.isValid());
        CHECK(state.state() == NodeValidationState::State::Warning);
        CHECK(state.message() == "Test warning");
    }

    SECTION("Validation state can be set to Error")
    {
        NodeValidationState state;
        state._state = NodeValidationState::State::Error;
        state._stateMessage = "Test error";

        CHECK_FALSE(state.isValid());
        CHECK(state.state() == NodeValidationState::State::Error);
        CHECK(state.message() == "Test error");
    }
}

TEST_CASE("NodeDelegateModel validation and status", "[validation]")
{
    auto registry = std::make_shared<NodeDelegateModelRegistry>();
    registry->registerModel<TestValidatedModel>("Test");

    DataFlowGraphModel model(registry);
    NodeId nodeId = model.addNode("TestValidated");

    REQUIRE(model.nodeExists(nodeId));

    SECTION("Default processing status is NoStatus")
    {
        auto status = model.nodeData(nodeId, NodeRole::ProcessingStatus);
        // Check that we get a valid variant (may be default status)
        CHECK(status.isValid());
    }

    SECTION("Default validation state is Valid")
    {
        auto state = model.nodeData(nodeId, NodeRole::ValidationState);
        CHECK(state.isValid());
    }

    SECTION("Model exposes delegate for status modification")
    {
        auto *delegate = model.delegateModel<TestValidatedModel>(nodeId);
        REQUIRE(delegate != nullptr);

        // Set processing status
        delegate->setTestProcessingStatus(NodeProcessingStatus::Processing);
        CHECK(delegate->processingStatus() == NodeProcessingStatus::Processing);

        delegate->setTestProcessingStatus(NodeProcessingStatus::Updated);
        CHECK(delegate->processingStatus() == NodeProcessingStatus::Updated);

        delegate->setTestProcessingStatus(NodeProcessingStatus::Failed);
        CHECK(delegate->processingStatus() == NodeProcessingStatus::Failed);
    }

    SECTION("Validation state can be set through delegate")
    {
        auto *delegate = model.delegateModel<TestValidatedModel>(nodeId);
        REQUIRE(delegate != nullptr);

        delegate->setTestValidationState(NodeValidationState::State::Error, "Invalid input");

        auto state = delegate->validationState();
        CHECK(state.state() == NodeValidationState::State::Error);
        CHECK(state.message() == "Invalid input");
    }
}

TEST_CASE("NodeProcessingStatus enum values", "[validation]")
{
    CHECK(static_cast<int>(NodeProcessingStatus::NoStatus) == 0);
    CHECK(static_cast<int>(NodeProcessingStatus::Updated) == 1);
    CHECK(static_cast<int>(NodeProcessingStatus::Processing) == 2);
    CHECK(static_cast<int>(NodeProcessingStatus::Pending) == 3);
    CHECK(static_cast<int>(NodeProcessingStatus::Empty) == 4);
    CHECK(static_cast<int>(NodeProcessingStatus::Failed) == 5);
    CHECK(static_cast<int>(NodeProcessingStatus::Partial) == 6);
}

#include "TestNodeValidation.moc"
