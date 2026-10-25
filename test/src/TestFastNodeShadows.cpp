#include "ApplicationSetup.hpp"
#include "TestGraphModel.hpp"

#include <catch2/catch.hpp>

#include <QtNodes/internal/AbstractNodeGeometry.hpp>
#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/DefaultNodePainter.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/NodeStyle.hpp>

#include <QImage>
#include <QJsonObject>
#include <QPainter>

TEST_CASE("Lightweight shadows are opt-in and survive style serialization", "[graphics][shadows]")
{
    auto app = applicationSetup();

    QtNodes::NodeStyle legacyStyle;
    legacyStyle.ShadowEnabled = true;
    CHECK_FALSE(legacyStyle.FastShadowEnabled);

    auto legacyJson = legacyStyle.toJson();
    QJsonObject legacyNodeJson = legacyJson.value("NodeStyle").toObject();
    legacyNodeJson.remove("FastShadowEnabled");
    legacyJson["NodeStyle"] = legacyNodeJson;
    CHECK_FALSE(QtNodes::NodeStyle(legacyJson).FastShadowEnabled);

    QtNodes::NodeStyle fastStyle;
    fastStyle.ShadowEnabled = true;
    fastStyle.FastShadowEnabled = true;
    CHECK(QtNodes::NodeStyle(fastStyle.toJson()).FastShadowEnabled);

    fastStyle.loadJson(legacyJson);
    CHECK_FALSE(fastStyle.FastShadowEnabled);
}

TEST_CASE("Shadow flags select the graphics effect", "[graphics][shadows]")
{
    bool const shadowEnabled = GENERATE(false, true);
    bool const fastShadowEnabled = GENERATE(false, true);
    CAPTURE(shadowEnabled, fastShadowEnabled);
    auto app = applicationSetup();

    QtNodes::NodeStyle style;
    style.ShadowEnabled = shadowEnabled;
    style.FastShadowEnabled = fastShadowEnabled;

    TestGraphModel model;
    auto const id = model.addNode("shadow");
    // Match DataFlowGraphModel and the documented NodeRole::Style representation.
    REQUIRE(model.setNodeData(id, QtNodes::NodeRole::Style, style.toJson().toVariantMap()));

    QtNodes::BasicGraphicsScene scene(model);
    auto *node = scene.nodeGraphicsObject(id);
    REQUIRE(node != nullptr);
    CHECK((node->graphicsEffect() != nullptr) == (shadowEnabled && !fastShadowEnabled));
}

TEST_CASE("Lightweight shadow fits within default node margins", "[graphics][shadows]")
{
    bool const vertical = GENERATE(false, true);
    bool const shadowEnabled = GENERATE(false, true);
    CAPTURE(vertical, shadowEnabled);
    auto app = applicationSetup();

    TestGraphModel model;
    auto const id = model.addNode("fast");

    QtNodes::NodeStyle style;
    style.ShadowEnabled = shadowEnabled;
    style.FastShadowEnabled = true;
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
    QRectF const shadowFootprint(-6, -6, size.width() + 20, size.height() + 20);
    CHECK(scene.nodeGeometry().boundingRect(id).contains(shadowFootprint));

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
