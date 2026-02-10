#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/internal/AbstractConnectionPainter.hpp>
#include <QtNodes/internal/AbstractNodePainter.hpp>
#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

#include <QPainter>
#include <QPixmap>

using QtNodes::AbstractConnectionPainter;
using QtNodes::AbstractNodePainter;
using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionId;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeId;
using QtNodes::NodeRole;

/// Custom node painter for testing
class TestNodePainter : public AbstractNodePainter
{
public:
    mutable int paintCallCount = 0;
    mutable NodeId lastPaintedNodeId = 0;

    void paint(QPainter *painter, NodeGraphicsObject &ngo) const override
    {
        paintCallCount++;
        lastPaintedNodeId = ngo.nodeId();

        // Simple paint implementation
        painter->setBrush(Qt::blue);
        painter->drawRect(0, 0, 100, 50);
    }
};

/// Custom connection painter for testing
class TestConnectionPainter : public AbstractConnectionPainter
{
public:
    mutable int paintCallCount = 0;

    void paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const override
    {
        paintCallCount++;

        QPen pen(Qt::red, 2);
        painter->setPen(pen);
        painter->drawLine(cgo.endPoint(QtNodes::PortType::Out), cgo.endPoint(QtNodes::PortType::In));
    }

    QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo) const override
    {
        QPainterPath path;
        path.moveTo(cgo.endPoint(QtNodes::PortType::Out));
        path.lineTo(cgo.endPoint(QtNodes::PortType::In));

        QPainterPathStroker stroker;
        stroker.setWidth(10.0);
        return stroker.createStroke(path);
    }
};

TEST_CASE("Custom painters registration", "[painters]")
{
    auto app = applicationSetup();

    TestGraphModel model;
    BasicGraphicsScene scene(model);

    SECTION("Scene has default painters initially")
    {
        // Scene should have valid painters
        AbstractNodePainter &nodePainter = scene.nodePainter();
        AbstractConnectionPainter &connPainter = scene.connectionPainter();

        // Basic check that painters exist (won't crash)
        CHECK(&nodePainter != nullptr);
        CHECK(&connPainter != nullptr);
    }

    SECTION("Custom node painter can be registered")
    {
        auto customPainter = std::make_unique<TestNodePainter>();
        TestNodePainter *painterPtr = customPainter.get();

        scene.setNodePainter(std::move(customPainter));

        // Verify the painter was set
        AbstractNodePainter &currentPainter = scene.nodePainter();
        CHECK(&currentPainter == painterPtr);
    }

    SECTION("Custom connection painter can be registered")
    {
        auto customPainter = std::make_unique<TestConnectionPainter>();
        TestConnectionPainter *painterPtr = customPainter.get();

        scene.setConnectionPainter(std::move(customPainter));

        // Verify the painter was set
        AbstractConnectionPainter &currentPainter = scene.connectionPainter();
        CHECK(&currentPainter == painterPtr);
    }

    SECTION("Painter replacement")
    {
        auto painter1 = std::make_unique<TestNodePainter>();
        auto painter2 = std::make_unique<TestNodePainter>();
        TestNodePainter *ptr2 = painter2.get();

        scene.setNodePainter(std::move(painter1));
        scene.setNodePainter(std::move(painter2));

        // Second painter should be active
        AbstractNodePainter &currentPainter = scene.nodePainter();
        CHECK(&currentPainter == ptr2);
    }
}

TEST_CASE("Custom painter invocation", "[painters]")
{
    auto app = applicationSetup();

    TestGraphModel model;
    BasicGraphicsScene scene(model);

    auto customNodePainter = std::make_unique<TestNodePainter>();
    TestNodePainter *nodePainterPtr = customNodePainter.get();
    scene.setNodePainter(std::move(customNodePainter));

    SECTION("Node painter is called when nodes are rendered")
    {
        NodeId nodeId = model.addNode("TestNode");
        model.setNodeData(nodeId, NodeRole::Position, QPointF(0, 0));

        // Force scene update
        QCoreApplication::processEvents();

        // Create a pixmap and render the scene to trigger painting
        QPixmap pixmap(200, 200);
        QPainter painter(&pixmap);
        scene.render(&painter);
        painter.end();

        // Painter should have been called at least once
        CHECK(nodePainterPtr->paintCallCount > 0);
        CHECK(nodePainterPtr->lastPaintedNodeId == nodeId);
    }
}
