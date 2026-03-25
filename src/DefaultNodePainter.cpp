#include "DefaultNodePainter.hpp"

#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "DataFlowGraphModel.hpp"
#include "GraphicsView.hpp"
#include "NodeRenderingUtils.hpp"
#include "NodeDelegateModel.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeState.hpp"
#include "StyleCollection.hpp"

#include <QtCore/QHash>
#include <QtGui/QImage>
#include <QtGui/QPainterPath>

#include <algorithm>
#include <cmath>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace QtNodes {

namespace {

void box_blur_alpha(QImage &img, int radius)
{
    const int w = img.width();
    const int h = img.height();
    if (w == 0 || h == 0 || radius <= 0) {
        return;
    }

    const int span = 2 * radius + 1;
    std::vector<uint8_t> buf(static_cast<std::size_t>(w) * h);

    // Read alpha from premultiplied ARGB scanlines.
    auto alpha_at = [&](int x, int y) -> int {
        return qAlpha(reinterpret_cast<QRgb const *>(img.constScanLine(y))[x]);
    };

    // Horizontal pass → buf
    for (int y = 0; y < h; ++y) {
        int sum = 0;
        for (int x = -radius; x <= radius; ++x) {
            sum += alpha_at(std::clamp(x, 0, w - 1), y);
        }
        buf[y * w] = static_cast<uint8_t>(sum / span);
        for (int x = 1; x < w; ++x) {
            sum += alpha_at(std::min(x + radius, w - 1), y);
            sum -= alpha_at(std::max(x - radius - 1, 0), y);
            buf[y * w + x] = static_cast<uint8_t>(sum / span);
        }
    }

    // Vertical pass → back into image
    for (int x = 0; x < w; ++x) {
        int sum = 0;
        for (int y = -radius; y <= radius; ++y) {
            sum += buf[std::clamp(y, 0, h - 1) * w + x];
        }
        reinterpret_cast<QRgb *>(img.scanLine(0))[x] = qPremultiply(qRgba(0, 0, 0, sum / span));
        for (int y = 1; y < h; ++y) {
            sum += buf[std::min(y + radius, h - 1) * w + x];
            sum -= buf[std::max(y - radius - 1, 0) * w + x];
            reinterpret_cast<QRgb *>(img.scanLine(y))[x] = qPremultiply(qRgba(0, 0, 0, sum / span));
        }
    }
}

QImage generate_shadow_atlas(QColor shadow_color, qreal dpr)
{
    const int phys = static_cast<int>(std::ceil(node_rendering::k_atlas_size * dpr));

    QImage img(phys, phys, QImage::Format_ARGB32_Premultiplied);
    img.setDevicePixelRatio(dpr);
    img.fill(Qt::transparent);

    // Draw an opaque rounded rect in the center of the atlas.
    {
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        QRectF body(node_rendering::k_shadow_margin,
                    node_rendering::k_shadow_margin,
                    node_rendering::k_body_size,
                    node_rendering::k_body_size);
        p.drawRoundedRect(body, node_rendering::k_node_radius, node_rendering::k_node_radius);
    }

    // Multi-pass box blur on the alpha channel.
    const int phys_blur = std::max(
        1,
        static_cast<int>(std::round(node_rendering::k_blur_radius * dpr)));
    for (int pass = 0; pass < node_rendering::k_blur_passes; ++pass) {
        box_blur_alpha(img, phys_blur);
    }

    // Tint with shadow color while preserving the style alpha and applying the
    // configured global strength multiplier.
    const int sr = shadow_color.red();
    const int sg = shadow_color.green();
    const int sb = shadow_color.blue();
    const int sa = shadow_color.alpha();
    for (int y = 0; y < phys; ++y) {
        auto *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < phys; ++x) {
            const int blurred_alpha = (qAlpha(line[x]) * sa) / 255;
            const int a = (blurred_alpha * node_rendering::k_shadow_opacity) / 255;
            line[x] = qPremultiply(qRgba(sr, sg, sb, a));
        }
    }

    return img;
}

struct Shadow_cache_key
{
    QRgb color;
    int dpr_micro;  // dpr * 1e6 as int for reliable comparison

    bool operator==(Shadow_cache_key const &o) const
    {
        return color == o.color && dpr_micro == o.dpr_micro;
    }
};

struct Shadow_cache_key_hash
{
    std::size_t operator()(Shadow_cache_key const &k) const
    {
        return std::hash<uint64_t>()(
            (static_cast<uint64_t>(k.color) << 32) |
            static_cast<uint32_t>(k.dpr_micro));
    }
};

std::unordered_map<Shadow_cache_key, QImage, Shadow_cache_key_hash> s_shadow_cache;
std::mutex s_shadow_cache_mutex;

QImage cached_shadow_atlas(QColor shadow_color, qreal dpr)
{
    std::lock_guard<std::mutex> lock(s_shadow_cache_mutex);

    Shadow_cache_key key{shadow_color.rgba(),
                         static_cast<int>(dpr * 1000000.0)};
    auto it = s_shadow_cache.find(key);
    if (it != s_shadow_cache.end()) {
        return it->second;
    }

    if (s_shadow_cache.size() >= 32) {
        // Arbitrary eviction is sufficient here; the cache is tiny.
        s_shadow_cache.erase(s_shadow_cache.begin());
    }

    return s_shadow_cache.emplace(key, generate_shadow_atlas(shadow_color, dpr))
        .first->second;
}

void draw_nine_slice_shadow(
    QPainter *painter,
    QColor shadow_color,
    QRectF const &node_rect)
{
    const qreal dpr = painter->device()
        ? painter->device()->devicePixelRatioF()
        : 1.0;
    QImage const atlas = cached_shadow_atlas(shadow_color, dpr);
    if (atlas.isNull()) {
        return;
    }

    // Margin in logical coords — covers the full blur transition
    // (outer falloff + inward ramp to plateau).
    const qreal m = node_rendering::k_shadow_margin;

    // Destination rect: node rect shifted by shadow offset, expanded by margin.
    const qreal dx = node_rect.x() + node_rendering::k_shadow_offset_x - m;
    const qreal dy = node_rect.y() + node_rendering::k_shadow_offset_y - m;
    const qreal dw = node_rect.width()  + 2.0 * m;
    const qreal dh = node_rect.height() + 2.0 * m;
    const qreal inner_w = dw - 2.0 * m;
    const qreal inner_h = dh - 2.0 * m;

    if (inner_w <= 0.0 || inner_h <= 0.0) {
        return;
    }

    // Source margin/body in logical atlas coords (atlas has DPR set).
    const qreal sm = m;                          // source margin
    const qreal sb = static_cast<qreal>(node_rendering::k_body_size);  // source body

    // Snap target rects to device pixels to prevent hairline gaps.
    QTransform const &dt = painter->deviceTransform();
    bool inv_ok = false;
    QTransform const inv = dt.inverted(&inv_ok);
    auto snap = [&](qreal lx, qreal ly, qreal lw, qreal lh) -> QRectF {
        if (!inv_ok) {
            return QRectF(lx, ly, lw, lh);
        }
        QPointF p0 = dt.map(QPointF(lx, ly));
        QPointF p1 = dt.map(QPointF(lx + lw, ly + lh));
        p0 = QPointF(std::round(p0.x()), std::round(p0.y()));
        p1 = QPointF(std::round(p1.x()), std::round(p1.y()));
        return QRectF(inv.map(p0), inv.map(p1));
    };

    // 9 source rects (logical coords in the atlas).
    const QRectF s_tl(0,        0,        sm, sm);
    const QRectF s_tc(sm,       0,        sb, sm);
    const QRectF s_tr(sm + sb,  0,        sm, sm);
    const QRectF s_ml(0,        sm,       sm, sb);
    const QRectF s_mc(sm,       sm,       sb, sb);
    const QRectF s_mr(sm + sb,  sm,       sm, sb);
    const QRectF s_bl(0,        sm + sb,  sm, sm);
    const QRectF s_bc(sm,       sm + sb,  sb, sm);
    const QRectF s_br(sm + sb,  sm + sb,  sm, sm);

    // 9 target rects (snapped to device pixels).
    painter->drawImage(snap(dx,                dy,                m,       m),       atlas, s_tl);
    painter->drawImage(snap(dx + m,            dy,                inner_w, m),       atlas, s_tc);
    painter->drawImage(snap(dx + m + inner_w,  dy,                m,       m),       atlas, s_tr);
    painter->drawImage(snap(dx,                dy + m,            m,       inner_h), atlas, s_ml);
    painter->drawImage(snap(dx + m,            dy + m,            inner_w, inner_h), atlas, s_mc);
    painter->drawImage(snap(dx + m + inner_w,  dy + m,            m,       inner_h), atlas, s_mr);
    painter->drawImage(snap(dx,                dy + m + inner_h,  m,       m),       atlas, s_bl);
    painter->drawImage(snap(dx + m,            dy + m + inner_h,  inner_w, m),       atlas, s_bc);
    painter->drawImage(snap(dx + m + inner_w,  dy + m + inner_h,  m,       m),       atlas, s_br);
}

// ============================================================================

GraphicsView *graphics_view(NodeGraphicsObject &ngo)
{
    if (auto *view = ngo.currentGraphicsView()) {
        return view;
    }

    if (!ngo.scene()) {
        return nullptr;
    }

    QList<QGraphicsView *> const views = ngo.scene()->views();
    for (QGraphicsView *view : views) {
        if (auto *graphicsView = qobject_cast<GraphicsView *>(view)) {
            return graphicsView;
        }
    }

    return nullptr;
}

bool should_draw_text_as_path(GraphicsView *view)
{
    if (!view) {
        return false;
    }

    switch (view->textRenderingPolicy()) {
    case GraphicsView::TextRenderingPolicy::QtText:
        return false;
    case GraphicsView::TextRenderingPolicy::PathWhenZooming:
        return view->isZoomAnimating();
    case GraphicsView::TextRenderingPolicy::PathAlways:
        return true;
    }

    return false;
}

void configure_text_painter(QPainter *painter, GraphicsView *view)
{
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    if (should_draw_text_as_path(view)) {
        return;
    }

    if (!view || !view->isZoomAnimating()) {
        return;
    }

    QFont font = painter->font();
    font.setHintingPreference(QFont::PreferNoHinting);
    painter->setFont(font);
}

// Paths are cached at the origin and the painter is translated to the
// draw position.  The cache key combines QFont::key() (which encodes
// family, size, weight, style, hinting, etc.) with the text string.
// For typical node scenes the cache holds ~15 entries and never evicts.
QHash<QString, QPainterPath> s_text_path_cache;
std::mutex s_text_path_cache_mutex;
std::unordered_map<Shadow_cache_key, QImage, Shadow_cache_key_hash> s_validation_icon_cache;
std::mutex s_validation_icon_cache_mutex;

QImage validation_icon(QIcon const &icon, QColor const &color, qreal dpr)
{
    std::lock_guard<std::mutex> lock(s_validation_icon_cache_mutex);

    Shadow_cache_key key{color.rgba(),
                         static_cast<int>(dpr * 1000000.0)};
    auto it = s_validation_icon_cache.find(key);
    if (it != s_validation_icon_cache.end()) {
        return it->second;
    }

    QImage image = node_rendering::render_icon_image(icon, QSize(16, 16), dpr);
    if (image.isNull()) {
        return image;
    }

    QPainter imgPainter(&image);
    imgPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    imgPainter.fillRect(QRect(QPoint(0, 0), QSize(16, 16)), color);
    imgPainter.end();

    if (s_validation_icon_cache.size() >= 32) {
        s_validation_icon_cache.erase(s_validation_icon_cache.begin());
    }

    return s_validation_icon_cache.emplace(key, std::move(image)).first->second;
}

void draw_text(
    QPainter *painter,
    GraphicsView *view,
    QPointF const &position,
    QString const &text,
    QColor const &color,
    QFont const &font)
{
    if (should_draw_text_as_path(view)) {
        QString const key = font.key() + text;
        QPainterPath path;

        {
            std::lock_guard<std::mutex> lock(s_text_path_cache_mutex);

            auto it = s_text_path_cache.constFind(key);
            if (it == s_text_path_cache.constEnd()) {
                if (s_text_path_cache.size() >= 500) {
                    // Arbitrary eviction keeps insertion cost predictable without LRU bookkeeping.
                    s_text_path_cache.erase(s_text_path_cache.begin());
                }

                QPainterPath new_path;
                new_path.addText(QPointF(0, 0), font, text);
                it = s_text_path_cache.insert(key, std::move(new_path));
            }

            path = *it;
        }

        painter->setPen(Qt::NoPen);
        painter->translate(position);
        painter->fillPath(path, color);
        painter->translate(-position);
        return;
    }

    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(position, text);
}

} // namespace

void DefaultNodePainter::paint(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    GraphicsView *view = graphics_view(ngo);

    std::optional<NodeStyle> fallback_style;
    NodeStyle const &style = node_rendering::resolved_node_style(model, nodeId, fallback_style);

    drawNodeRect(painter, ngo, style);

    drawConnectionPoints(painter, ngo, style);

    drawFilledConnectionPoints(painter, ngo, style);

    drawNodeCaption(painter, ngo, style, view);

    drawEntryLabels(painter, ngo, style, view);

    drawProcessingIndicator(painter, ngo);

    drawResizeRect(painter, ngo);

    drawValidationIcon(painter, ngo, style);
}

void DefaultNodePainter::drawNodeRect(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle) const
{
    AbstractGraphModel &model = ngo.graphModel();

    NodeId const nodeId = ngo.nodeId();

    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    QVariant var = model.nodeData(nodeId, NodeRole::ValidationState);
    bool invalid = false;

    QColor color = ngo.isSelected() ? nodeStyle.selectedBoundaryColor()
                                    : nodeStyle.normalBoundaryColor();

    if (var.canConvert<NodeValidationState>()) {
        auto state = var.value<NodeValidationState>();
        switch (state.state()) {
        case NodeValidationState::State::Error: {
            invalid = true;
            color = nodeStyle.errorColor();
        } break;
        case NodeValidationState::State::Warning: {
            invalid = true;
            color = nodeStyle.warningColor();
        } break;
        default:
            break;
        }
    }

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 3.0;

    // 9-slice shadow: a precomputed blurred atlas is sliced into 9 tiles
    // and stretched to fit the node.  One atlas per (color, DPR), size-
    // independent.  Much faster than QGraphicsDropShadowEffect and smoother
    // than stacked translucent rounded rects.
    if (nodeStyle.shadowEnabled()) {
        draw_nine_slice_shadow(painter, nodeStyle.shadowColor(), boundary);
    }

    if (ngo.nodeState().hovered()) {
        painter->setPen(QPen(color, nodeStyle.hoveredPenWidth()));
    }
    else {
        painter->setPen(QPen(color, nodeStyle.penWidth()));
    }

    if (invalid) {
        painter->setBrush(color);
    }
    else {
        QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, size.height()));
        gradient.setColorAt(0.0, nodeStyle.gradientColor0());
        gradient.setColorAt(0.10, nodeStyle.gradientColor1());
        gradient.setColorAt(0.90, nodeStyle.gradientColor2());
        gradient.setColorAt(1.0, nodeStyle.gradientColor3());
        painter->setBrush(gradient);
    }

    painter->drawRoundedRect(boundary, radius, radius);
}

void DefaultNodePainter::drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    auto const &connectionStyle = StyleCollection::connectionStyle();

    float diameter = nodeStyle.connectionPointDiameter();
    auto reducedDiameter = diameter * 0.6;

    for (PortType portType : {PortType::Out, PortType::In}) {
        size_t const n = model.nodeData(nodeId, portCountRole(portType)).toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &dataType = model.portData(nodeId, portType, portIndex, PortRole::DataType)
                                       .value<NodeDataType>();

            double r = 1.0;

            NodeState const &state = ngo.nodeState();

            if (auto const *cgo = state.connectionForReaction()) {
                PortType requiredPort = cgo->connectionState().requiredPort();

                if (requiredPort == portType) {
                    ConnectionId possibleConnectionId = makeCompleteConnectionId(cgo->connectionId(),
                                                                                 nodeId,
                                                                                 portIndex);

                    bool const possible = model.connectionPossible(possibleConnectionId);

                    auto cp = cgo->sceneTransform().map(cgo->endPoint(requiredPort));
                    cp = ngo.sceneTransform().inverted().map(cp);

                    auto diff = cp - p;
                    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

                    if (possible) {
                        double const thres = 40.0;
                        r = (dist < thres) ? (2.0 - dist / thres) : 1.0;
                    } else {
                        double const thres = 80.0;
                        r = (dist < thres) ? (dist / thres) : 1.0;
                    }
                }
            }

            if (connectionStyle.useDataDefinedColors()) {
                painter->setBrush(connectionStyle.normalColor(dataType.id));
            } else {
                painter->setBrush(nodeStyle.connectionPointColor());
            }

            painter->drawEllipse(p, reducedDiameter * r, reducedDiameter * r);
        }
    }

    if (ngo.nodeState().connectionForReaction()) {
        ngo.nodeState().resetConnectionForReaction();
    }
}

void DefaultNodePainter::drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    auto diameter = nodeStyle.connectionPointDiameter();

    for (PortType portType : {PortType::Out, PortType::In}) {
        size_t const n = model.nodeData(nodeId, portCountRole(portType)).toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &connected = model.connections(nodeId, portType, portIndex);

            if (!connected.empty()) {
                auto const &dataType = model
                                           .portData(nodeId, portType, portIndex, PortRole::DataType)
                                           .value<NodeDataType>();

                auto const &connectionStyle = StyleCollection::connectionStyle();
                if (connectionStyle.useDataDefinedColors()) {
                    QColor const c = connectionStyle.normalColor(dataType.id);
                    painter->setPen(c);
                    painter->setBrush(c);
                } else {
                    painter->setPen(nodeStyle.filledConnectionPointColor());
                    painter->setBrush(nodeStyle.filledConnectionPointColor());
                }

                painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
            }
        }
    }
}

void DefaultNodePainter::drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle, GraphicsView *view) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (!model.nodeData(nodeId, NodeRole::CaptionVisible).toBool())
        return;

    QString const name = model.nodeData(nodeId, NodeRole::Caption).toString();

    QFont f = painter->font();
    f.setBold(true);
    if (!should_draw_text_as_path(view) && view && view->isZoomAnimating()) {
        f.setHintingPreference(QFont::PreferNoHinting);
    }
    else {
        f.setHintingPreference(QFont::PreferDefaultHinting);
    }

    QPointF position = geometry.captionPosition(nodeId);

    painter->setRenderHint(QPainter::TextAntialiasing, true);
    draw_text(painter, view, position, name, nodeStyle.fontColor(), f);

    f.setBold(false);
    painter->setFont(f);
}

void DefaultNodePainter::drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle, GraphicsView *view) const
{
    configure_text_painter(painter, view);

    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    for (PortType portType : {PortType::Out, PortType::In}) {
        unsigned int n = model.nodeData<unsigned int>(nodeId, portCountRole(portType));

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            auto const &connected = model.connections(nodeId, portType, portIndex);

            QPointF p = geometry.portTextPosition(nodeId, portType, portIndex);

            if (connected.empty())
                painter->setPen(nodeStyle.fontColorFaded());
            else
                painter->setPen(nodeStyle.fontColor());

            QString s;

            if (model.portData<bool>(nodeId, portType, portIndex, PortRole::CaptionVisible)) {
                s = model.portData<QString>(nodeId, portType, portIndex, PortRole::Caption);
            } else {
                auto portData = model.portData(nodeId, portType, portIndex, PortRole::DataType);

                s = portData.value<NodeDataType>().name;
            }

            QColor const textColor = connected.empty() ? nodeStyle.fontColorFaded()
                                                       : nodeStyle.fontColor();
            draw_text(painter, view, p, s, textColor, painter->font());
        }
    }
}

void DefaultNodePainter::drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (model.nodeFlags(nodeId) & NodeFlag::Resizable) {
        painter->setBrush(Qt::gray);

        painter->drawEllipse(geometry.resizeHandleRect(nodeId));
    }
}

void DefaultNodePainter::drawProcessingIndicator(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();

    auto *dfModel = dynamic_cast<DataFlowGraphModel *>(&model);
    if (!dfModel)
        return;

    auto *delegate = dfModel->delegateModel<NodeDelegateModel>(nodeId);
    if (!delegate)
        return;

    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    qreal const dpr = painter->device()
        ? painter->device()->devicePixelRatioF()
        : 1.0;
    QImage const image = delegate->processingStatusImage(dpr);
    if (image.isNull())
        return;

    ProcessingIconStyle const iconStyle = delegate->processingIconStyle();

    qreal iconSize = iconStyle._size;
    qreal margin = iconStyle._margin;

    // Determine position, avoiding conflict with resize handle
    ProcessingIconPos pos = iconStyle._pos;
    bool isResizable = model.nodeFlags(nodeId) & NodeFlag::Resizable;
    if (isResizable && pos == ProcessingIconPos::BottomRight) {
        pos = ProcessingIconPos::BottomLeft;
    }

    qreal x = margin;
    if (pos == ProcessingIconPos::BottomRight) {
        x = size.width() - iconSize - margin;
    }

    QRectF const targetRect(x, size.height() - iconSize - margin, iconSize, iconSize);
    qreal const image_dpr = image.devicePixelRatio();
    QRectF const sourceRect(QPointF(0, 0),
                            QSizeF(image.width() / image_dpr,
                                   image.height() / image_dpr));
    painter->drawImage(targetRect, image, sourceRect);
}

void DefaultNodePainter::drawValidationIcon(QPainter *painter, NodeGraphicsObject &ngo, NodeStyle const &nodeStyle) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QVariant var = model.nodeData(nodeId, NodeRole::ValidationState);
    if (!var.canConvert<NodeValidationState>())
        return;

    auto state = var.value<NodeValidationState>();
    if (state.isValid())
        return;

    QSize size = geometry.size(nodeId);

    QSize const iconSize(16, 16);

    QColor color = (state.state() == NodeValidationState::State::Error) ? nodeStyle.errorColor()
                                                                        : nodeStyle.warningColor();
    qreal const dpr = painter->device()
        ? painter->device()->devicePixelRatioF()
        : 1.0;
    QImage const image = validation_icon(_toolTipIcon, color, dpr);
    if (image.isNull()) {
        return;
    }

    QPointF center(size.width(), 0.0);
    center += QPointF(iconSize.width() / 2.0, -iconSize.height() / 2.0);

    QRectF const targetRect(center.x() - iconSize.width() / 2.0,
                            center.y() - iconSize.height() / 2.0,
                            iconSize.width(),
                            iconSize.height());
    painter->drawImage(targetRect, image, QRectF(QPointF(0, 0), QSizeF(iconSize)));
}

} // namespace QtNodes
