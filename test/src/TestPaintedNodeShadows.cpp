#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/internal/AbstractNodeGeometry.hpp>
#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/DefaultNodePainter.hpp>
#include <QtNodes/internal/GraphicsView.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/NodeStyle.hpp>

#include <QApplication>
#include <QImage>
#include <QJsonObject>
#include <QPainter>

TEST_CASE("Painted shadows are on by default and a style JSON without the key keeps the "
          "graphics effect",
          "[graphics][shadows]")
{
    auto app = applicationSetup();

    QtNodes::NodeStyle legacyStyle;
    legacyStyle.ShadowEnabled = true;
    CHECK(legacyStyle.PaintedShadowEnabled);

    auto legacyJson = legacyStyle.toJson();
    QJsonObject legacyNodeJson = legacyJson.value("NodeStyle").toObject();
    legacyNodeJson.remove("PaintedShadowEnabled");
    legacyJson["NodeStyle"] = legacyNodeJson;
    CHECK_FALSE(QtNodes::NodeStyle(legacyJson).PaintedShadowEnabled);

    QtNodes::NodeStyle paintedStyle;
    paintedStyle.ShadowEnabled = true;
    paintedStyle.PaintedShadowEnabled = true;
    CHECK(QtNodes::NodeStyle(paintedStyle.toJson()).PaintedShadowEnabled);

    paintedStyle.loadJson(legacyJson);
    CHECK_FALSE(paintedStyle.PaintedShadowEnabled);
}

TEST_CASE("The graphics effect exists only if the shadow is on and the painted shadow is off",
          "[graphics][shadows]")
{
    bool const shadowEnabled = GENERATE(false, true);
    bool const paintedShadowEnabled = GENERATE(false, true);
    CAPTURE(shadowEnabled, paintedShadowEnabled);
    auto app = applicationSetup();

    QtNodes::NodeStyle style;
    style.ShadowEnabled = shadowEnabled;
    style.PaintedShadowEnabled = paintedShadowEnabled;

    TestGraphModel model;
    auto const id = model.addNode("shadow");
    // Match DataFlowGraphModel and the documented NodeRole::Style representation.
    REQUIRE(model.setNodeData(id, QtNodes::NodeRole::Style, style.toJson().toVariantMap()));

    QtNodes::BasicGraphicsScene scene(model);
    auto *node = scene.nodeGraphicsObject(id);
    REQUIRE(node != nullptr);
    CHECK((node->graphicsEffect() != nullptr) == (shadowEnabled && !paintedShadowEnabled));
}

TEST_CASE("The painted shadow footprint fits the node margins in both orientations",
          "[graphics][shadows]")
{
    bool const vertical = GENERATE(false, true);
    CAPTURE(vertical);
    auto app = applicationSetup();

    TestGraphModel model;
    auto const id = model.addNode("painted");

    QtNodes::BasicGraphicsScene scene(model);
    scene.setOrientation(vertical ? Qt::Vertical : Qt::Horizontal);

    QSize const size = scene.nodeGeometry().size(id);
    REQUIRE(size.width() > 0);
    REQUIRE(size.height() > 0);

    // The shadow reaches 11 px above and left, and 19 px below and right of the node body.
    QRectF const shadowFootprint(-11, -11, size.width() + 30, size.height() + 30);
    CHECK(scene.nodeGeometry().boundingRect(id).contains(shadowFootprint));
}

TEST_CASE(
    "The painted shadow draws only inside the node margins and only when the shadow is on",
    "[graphics][shadows]")
{
    bool const vertical = GENERATE(false, true);
    bool const shadowEnabled = GENERATE(false, true);
    CAPTURE(vertical, shadowEnabled);
    auto app = applicationSetup();

    TestGraphModel model;
    auto const id = model.addNode("painted");

    QtNodes::NodeStyle style;
    style.ShadowEnabled = shadowEnabled;
    style.PaintedShadowEnabled = true;
    style.ShadowColor = QColor(0, 0, 0);
    REQUIRE(model.setNodeData(id, QtNodes::NodeRole::Style, style.toJson().toVariantMap()));

    QtNodes::BasicGraphicsScene scene(model);
    scene.setOrientation(vertical ? Qt::Vertical : Qt::Horizontal);
    auto *node = scene.nodeGraphicsObject(id);
    REQUIRE(node != nullptr);

    QSize const size = scene.nodeGeometry().size(id);
    REQUIRE(size.width() > 0);
    REQUIRE(size.height() > 0);

    // The default horizontal and vertical geometries reserve 20 px per side.
    QImage image(size.width() + 60, size.height() + 60, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.translate(25, 25);
    QTransform const transform = painter.transform();
    auto const renderHints = painter.renderHints();
    QtNodes::DefaultNodePainter nodePainter;
    nodePainter.drawNodeRect(&painter, *node);
    CHECK(painter.transform() == transform);
    CHECK(painter.renderHints() == renderHints);
    CHECK(painter.pen().style() != Qt::NoPen);
    CHECK(painter.brush().style() == Qt::LinearGradientPattern);
    painter.end();

    int const midX = 25 + size.width() / 2;
    CHECK((qAlpha(image.pixel(midX, 25 + size.height() + 6)) > 0) == shadowEnabled);
    CHECK(qAlpha(image.pixel(midX, 25 + size.height() + 19)) == 0);

    QRectF const bounds = scene.nodeGeometry().boundingRect(id).translated(25, 25);
    unsigned int pixelsOutsideBounds = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(image.pixel(x, y)) != 0 && !bounds.contains(QPointF(x, y))) {
                ++pixelsOutsideBounds;
            }
        }
    }
    CHECK(pixelsOutsideBounds == 0);
}

namespace {
// Grabs a view that is zoomed in on a node. The node is larger than the view.
// Returns the grabbed image and the part of the view that shows the node body.
QImage grabZoomedNode(bool shadowEnabled, bool paintedShadowEnabled, QRect &bodyInView)
{
    QtNodes::NodeStyle style;
    style.ShadowEnabled = shadowEnabled;
    style.PaintedShadowEnabled = paintedShadowEnabled;

    TestGraphModel model;
    auto const id = model.addNode("zoom");
    REQUIRE(model.setNodeData(id, QtNodes::NodeRole::Style, style.toJson().toVariantMap()));

    QtNodes::BasicGraphicsScene scene(model);
    QtNodes::GraphicsView view(&scene);
    view.resize(400, 300);
    view.show();

    QSize const size = scene.nodeGeometry().size(id);
    view.resetTransform();
    view.scale(8.0, 8.0);
    view.centerOn(QPointF(size.width() / 2.0, size.height() / 2.0));
    QApplication::processEvents();

    QRect const nodeInView = view.mapFromScene(QRectF(QPointF(0, 0), size)).boundingRect();
    // Stay a few pixels away from the node edge, where the shadow and the border are.
    bodyInView = nodeInView.adjusted(4, 4, -4, -4).intersected(view.viewport()->rect());
    return view.grab().toImage();
}
} // namespace

TEST_CASE("A node larger than the view is drawn completely when the view is zoomed in",
          "[graphics][shadows]")
{
    auto app = applicationSetup();

    QRect body;
    QRect bodyWithoutShadow;
    QImage const withoutShadow = grabZoomedNode(false, false, bodyWithoutShadow);
    QImage const withShadow = grabZoomedNode(true, true, body);

    REQUIRE(body == bodyWithoutShadow);
    // The node must fill most of the view, otherwise the test does not prove anything.
    REQUIRE(body.width() * body.height() > 100 * 100);

    unsigned int differentPixels = 0;
    for (int y = body.top(); y <= body.bottom(); ++y) {
        for (int x = body.left(); x <= body.right(); ++x) {
            if (withShadow.pixel(x, y) != withoutShadow.pixel(x, y)) {
                ++differentPixels;
            }
        }
    }
    CHECK(differentPixels == 0);
}
