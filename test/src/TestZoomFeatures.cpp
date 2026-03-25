#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <catch2/catch_all.hpp>

#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

#include <QSignalSpy>
#include <QTest>
#include <QApplication>
#include <QWheelEvent>

#include <vector>

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeId;
using QtNodes::NodeRole;

TEST_CASE("GraphicsView scale range", "[zoom]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    SECTION("Default scale range allows unlimited zoom")
    {
        // By default, scale range should not limit zooming
        double initialScale = view.getScale();
        CHECK(initialScale > 0);

        // Zoom in multiple times
        for (int i = 0; i < 10; ++i) {
            view.scaleUp();
        }

        CHECK(view.getScale() > initialScale);
    }

    SECTION("Scale range can be set with minimum and maximum")
    {
        view.setScaleRange(0.5, 2.0);

        // Set scale to middle value
        view.setupScale(1.0);
        CHECK(view.getScale() == Approx(1.0).epsilon(0.01));

        // Try to zoom out beyond minimum
        view.setupScale(0.1);
        CHECK(view.getScale() >= 0.5);

        // Try to zoom in beyond maximum
        view.setupScale(5.0);
        CHECK(view.getScale() <= 2.0);
    }

    SECTION("Scale range can be set with ScaleRange struct")
    {
        GraphicsView::ScaleRange range{0.25, 4.0};
        view.setScaleRange(range);

        view.setupScale(0.1);
        CHECK(view.getScale() >= 0.25);

        view.setupScale(10.0);
        CHECK(view.getScale() <= 4.0);
    }

    SECTION("Smooth wheel zoom keeps the cursor anchor stable")
    {
        view.setupScale(1.0);

        QPointF const pivot(321.25, 247.75);
        bool invertible = false;
        QTransform const invertedTransform = view.viewportTransform().inverted(&invertible);
        REQUIRE(invertible);

        QPointF const trackedScenePoint = invertedTransform.map(pivot);

        QWheelEvent wheelEvent(pivot,
                               view.mapToGlobal(pivot.toPoint()),
                               QPoint(0, 0),
                               QPoint(0, 120),
                               Qt::NoButton,
                               Qt::NoModifier,
                               Qt::ScrollPhase::NoScrollPhase,
                               false);
        QApplication::sendEvent(view.viewport(), &wheelEvent);
        QTest::qWait(20);

        QPointF const mappedPivot = view.viewportTransform().map(trackedScenePoint);
        CHECK(mappedPivot.x() == Approx(pivot.x()).margin(1.0));
        CHECK(mappedPivot.y() == Approx(pivot.y()).margin(1.0));
    }

    SECTION("Smooth wheel zoom settles back to a clean transform")
    {
        QWheelEvent wheelEvent(QPointF(320.0, 240.0),
                               view.mapToGlobal(QPoint(320, 240)),
                               QPoint(0, 0),
                               QPoint(0, 120),
                               Qt::NoButton,
                               Qt::NoModifier,
                               Qt::ScrollPhase::NoScrollPhase,
                               false);
        QApplication::sendEvent(view.viewport(), &wheelEvent);
        QTest::qWait(500);

        CHECK(view.transform().dx() == Approx(0.0).margin(0.01));
        CHECK(view.transform().dy() == Approx(0.0).margin(0.01));
    }

    SECTION("Smooth wheel zoom math is invariant to timer cadence")
    {
        struct Zoom_state
        {
            double scale = 1.0;
            double velocity = 0.0;
        };

        auto advanceZoom = [](double initialVelocity, std::vector<double> const &elapsedSteps) {
            Zoom_state state{1.0, initialVelocity};
            for (double const elapsedStep : elapsedSteps) {
                state.scale *= GraphicsView::zoomAnimationScaleFactor(state.velocity, elapsedStep);
                state.velocity = GraphicsView::zoomAnimationVelocityAfter(state.velocity, elapsedStep);
            }
            return state;
        };

        Zoom_state const singleGap = advanceZoom(4.0, {10.0});
        Zoom_state const splitGap = advanceZoom(4.0, {4.0, 6.0});
        Zoom_state const fineSteps = advanceZoom(4.0, {1.0, 1.0, 1.0, 1.0, 1.0,
                                                       1.0, 1.0, 1.0, 1.0, 1.0});

        CHECK(singleGap.scale == Approx(splitGap.scale).epsilon(1e-10));
        CHECK(singleGap.velocity == Approx(splitGap.velocity).epsilon(1e-10));
        CHECK(singleGap.scale == Approx(fineSteps.scale).epsilon(1e-10));
        CHECK(singleGap.velocity == Approx(fineSteps.velocity).epsilon(1e-10));
    }
}

TEST_CASE("GraphicsView node cache policy", "[zoom]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    NodeId const nodeId = model->addNode("Node1");
    QCoreApplication::processEvents();

    auto *nodeGraphics = scene.nodeGraphicsObject(nodeId);
    REQUIRE(nodeGraphics != nullptr);

    SECTION("Rasterization policy toggles node cache mode")
    {
        view.setRasterizationPolicy(GraphicsView::RasterizationPolicy::Crisp);
        QCoreApplication::processEvents();
        CHECK(nodeGraphics->cacheMode() == QGraphicsItem::DeviceCoordinateCache);

        view.setRasterizationPolicy(GraphicsView::RasterizationPolicy::Consistent);
        QCoreApplication::processEvents();
        CHECK(nodeGraphics->cacheMode() == QGraphicsItem::NoCache);
    }

    SECTION("Smooth zoom disables node cache while animating")
    {
        view.setRasterizationPolicy(GraphicsView::RasterizationPolicy::Crisp);
        QCoreApplication::processEvents();
        REQUIRE(nodeGraphics->cacheMode() == QGraphicsItem::DeviceCoordinateCache);

        QWheelEvent wheelEvent(QPointF(320.0, 240.0),
                               view.mapToGlobal(QPoint(320, 240)),
                               QPoint(0, 0),
                               QPoint(0, 120),
                               Qt::NoButton,
                               Qt::NoModifier,
                               Qt::ScrollPhase::NoScrollPhase,
                               false);
        QApplication::sendEvent(view.viewport(), &wheelEvent);
        QCoreApplication::processEvents();

        CHECK(view.isZoomAnimating());
        CHECK(nodeGraphics->cacheMode() == QGraphicsItem::NoCache);

        QTest::qWait(500);
        CHECK_FALSE(view.isZoomAnimating());
        CHECK(nodeGraphics->cacheMode() == QGraphicsItem::DeviceCoordinateCache);
    }

    SECTION("Shared scenes keep node cache disabled")
    {
        GraphicsView secondView(&scene);
        secondView.resize(800, 600);
        secondView.show();
        REQUIRE(QTest::qWaitForWindowExposed(&secondView));

        view.setRasterizationPolicy(GraphicsView::RasterizationPolicy::Crisp);
        secondView.setRasterizationPolicy(GraphicsView::RasterizationPolicy::Crisp);
        QCoreApplication::processEvents();

        CHECK(nodeGraphics->cacheMode() == QGraphicsItem::NoCache);
    }
}

TEST_CASE("scaleChanged signal", "[zoom]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    SECTION("Signal emitted on scale change")
    {
        QSignalSpy spy(&view, &GraphicsView::scaleChanged);

        view.scaleUp();
        QCoreApplication::processEvents();

        // Signal should have been emitted
        CHECK(spy.count() >= 1);

        // Check signal argument
        if (spy.count() > 0) {
            QList<QVariant> arguments = spy.takeFirst();
            double scale = arguments.at(0).toDouble();
            CHECK(scale > 0);
        }
    }

    SECTION("Signal emitted with correct scale value")
    {
        QSignalSpy spy(&view, &GraphicsView::scaleChanged);

        view.setupScale(1.5);
        QCoreApplication::processEvents();

        CHECK(spy.count() >= 1);

        if (spy.count() > 0) {
            QList<QVariant> arguments = spy.takeLast();
            double scale = arguments.at(0).toDouble();
            CHECK(scale == Approx(1.5).epsilon(0.01));
        }
    }
}

TEST_CASE("Zoom fit operations", "[zoom]")
{
    auto app = applicationSetup();

    auto model = std::make_shared<TestGraphModel>();
    BasicGraphicsScene scene(*model);
    GraphicsView view(&scene);

    view.resize(800, 600);
    view.show();
    REQUIRE(QTest::qWaitForWindowExposed(&view));

    SECTION("zoomFitAll with nodes")
    {
        // Create some nodes spread across the scene
        NodeId node1 = model->addNode("Node1");
        model->setNodeData(node1, NodeRole::Position, QPointF(-500, -500));

        NodeId node2 = model->addNode("Node2");
        model->setNodeData(node2, NodeRole::Position, QPointF(500, 500));

        QCoreApplication::processEvents();

        double scaleBefore = view.getScale();

        // Fit all nodes
        view.zoomFitAll();
        QCoreApplication::processEvents();

        // Scale should have changed (either up or down to fit)
        double scaleAfter = view.getScale();
        // Just check we didn't crash and scale is still valid
        CHECK(scaleAfter > 0);
    }

    SECTION("zoomFitSelected with selected nodes")
    {
        // Create nodes
        NodeId node1 = model->addNode("Node1");
        model->setNodeData(node1, NodeRole::Position, QPointF(0, 0));

        NodeId node2 = model->addNode("Node2");
        model->setNodeData(node2, NodeRole::Position, QPointF(200, 200));

        QCoreApplication::processEvents();

        // Select one node
        auto *nodeGraphics = scene.nodeGraphicsObject(node1);
        REQUIRE(nodeGraphics != nullptr);
        nodeGraphics->setSelected(true);

        QCoreApplication::processEvents();

        // Fit selected
        view.zoomFitSelected();
        QCoreApplication::processEvents();

        // Just check we didn't crash
        CHECK(view.getScale() > 0);
    }

    SECTION("zoomFitAll with empty scene")
    {
        // Empty scene - should not crash
        view.zoomFitAll();
        QCoreApplication::processEvents();

        CHECK(view.getScale() > 0);
    }

    SECTION("zoomFitSelected with no selection")
    {
        NodeId node1 = model->addNode("Node1");
        model->setNodeData(node1, NodeRole::Position, QPointF(0, 0));

        QCoreApplication::processEvents();

        // Don't select anything
        view.zoomFitSelected();
        QCoreApplication::processEvents();

        // Should not crash
        CHECK(view.getScale() > 0);
    }
}
