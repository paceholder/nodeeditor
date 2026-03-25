#include "NodeStyle.hpp"

#include "StyleCollection.hpp"

#include <QtCore/QJsonObject>

#include <QtCore/QDebug>

using QtNodes::NodeStyle;
using namespace QtNodes::detail;

inline void initResources()
{
    Q_INIT_RESOURCE(resources);
}

NodeStyle::NodeStyle()
{
    // Explicit resources inialization for preventing the static initialization
    // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
    initResources();

    // Initialize status icons after resources are loaded
    _statusUpdated = QIcon(":/status_icons/updated.svg");
    _statusProcessing = QIcon(":/status_icons/processing.svg");
    _statusPending = QIcon(":/status_icons/pending.svg");
    _statusInvalid = QIcon(":/status_icons/failed.svg");
    _statusEmpty = QIcon(":/status_icons/empty.svg");
    _statusPartial = QIcon(":/status_icons/partial.svg");

    // This configuration is stored inside the compiled unit and is loaded statically
    loadJsonFile(":DefaultStyle.json");
}

NodeStyle::NodeStyle(QString jsonText)
{
    loadJsonText(jsonText);
}

NodeStyle::NodeStyle(QJsonObject const &json)
{
    loadJson(json);
}

void NodeStyle::setNodeStyle(QString jsonText)
{
    NodeStyle style(jsonText);

    StyleCollection::setNodeStyle(style);
}

void NodeStyle::loadJson(QJsonObject const &json)
{
    QJsonObject obj = json["NodeStyle"].toObject();

    readColor(obj, "NormalBoundaryColor", _NormalBoundaryColor);
    readColor(obj, "SelectedBoundaryColor", _SelectedBoundaryColor);
    readColor(obj, "GradientColor0", _GradientColor0);
    readColor(obj, "GradientColor1", _GradientColor1);
    readColor(obj, "GradientColor2", _GradientColor2);
    readColor(obj, "GradientColor3", _GradientColor3);
    readColor(obj, "ShadowColor", _ShadowColor);
    readBool(obj, "ShadowEnabled", _ShadowEnabled);
    readColor(obj, "FontColor", _FontColor);
    readColor(obj, "FontColorFaded", _FontColorFaded);
    readColor(obj, "ConnectionPointColor", _ConnectionPointColor);
    readColor(obj, "FilledConnectionPointColor", _FilledConnectionPointColor);
    readColor(obj, "WarningColor", _WarningColor);
    readColor(obj, "ErrorColor", _ErrorColor);

    readFloat(obj, "PenWidth", _PenWidth);
    readFloat(obj, "HoveredPenWidth", _HoveredPenWidth);
    readFloat(obj, "ConnectionPointDiameter", _ConnectionPointDiameter);

    readFloat(obj, "Opacity", _Opacity);
}

QJsonObject NodeStyle::toJson() const
{
    QJsonObject obj;

    writeColor(obj, "NormalBoundaryColor", _NormalBoundaryColor);
    writeColor(obj, "SelectedBoundaryColor", _SelectedBoundaryColor);
    writeColor(obj, "GradientColor0", _GradientColor0);
    writeColor(obj, "GradientColor1", _GradientColor1);
    writeColor(obj, "GradientColor2", _GradientColor2);
    writeColor(obj, "GradientColor3", _GradientColor3);
    writeColor(obj, "ShadowColor", _ShadowColor);
    writeBool(obj, "ShadowEnabled", _ShadowEnabled);
    writeColor(obj, "FontColor", _FontColor);
    writeColor(obj, "FontColorFaded", _FontColorFaded);
    writeColor(obj, "ConnectionPointColor", _ConnectionPointColor);
    writeColor(obj, "FilledConnectionPointColor", _FilledConnectionPointColor);
    writeColor(obj, "WarningColor", _WarningColor);
    writeColor(obj, "ErrorColor", _ErrorColor);

    writeFloat(obj, "PenWidth", _PenWidth);
    writeFloat(obj, "HoveredPenWidth", _HoveredPenWidth);
    writeFloat(obj, "ConnectionPointDiameter", _ConnectionPointDiameter);

    writeFloat(obj, "Opacity", _Opacity);

    QJsonObject root;
    root["NodeStyle"] = obj;

    return root;
}

void NodeStyle::setBackgroundColor(QColor const &color)
{
    _GradientColor0 = color;
    _GradientColor1 = color;
    _GradientColor2 = color;
    _GradientColor3 = color;
}

QColor NodeStyle::backgroundColor() const
{
    return _GradientColor0;
}

QColor NodeStyle::normalBoundaryColor() const { return _NormalBoundaryColor; }
QColor NodeStyle::selectedBoundaryColor() const { return _SelectedBoundaryColor; }
QColor NodeStyle::gradientColor0() const { return _GradientColor0; }
QColor NodeStyle::gradientColor1() const { return _GradientColor1; }
QColor NodeStyle::gradientColor2() const { return _GradientColor2; }
QColor NodeStyle::gradientColor3() const { return _GradientColor3; }
QColor NodeStyle::shadowColor() const { return _ShadowColor; }
bool NodeStyle::shadowEnabled() const { return _ShadowEnabled; }
QColor NodeStyle::fontColor() const { return _FontColor; }
QColor NodeStyle::fontColorFaded() const { return _FontColorFaded; }
QColor NodeStyle::connectionPointColor() const { return _ConnectionPointColor; }
QColor NodeStyle::filledConnectionPointColor() const { return _FilledConnectionPointColor; }
QColor NodeStyle::warningColor() const { return _WarningColor; }
QColor NodeStyle::errorColor() const { return _ErrorColor; }
QColor NodeStyle::toolTipIconColor() const { return _ToolTipIconColor; }
float NodeStyle::penWidth() const { return _PenWidth; }
float NodeStyle::hoveredPenWidth() const { return _HoveredPenWidth; }
float NodeStyle::connectionPointDiameter() const { return _ConnectionPointDiameter; }
float NodeStyle::opacity() const { return _Opacity; }
QIcon const &NodeStyle::statusUpdated() const { return _statusUpdated; }
QIcon const &NodeStyle::statusProcessing() const { return _statusProcessing; }
QIcon const &NodeStyle::statusPending() const { return _statusPending; }
QIcon const &NodeStyle::statusInvalid() const { return _statusInvalid; }
QIcon const &NodeStyle::statusEmpty() const { return _statusEmpty; }
QIcon const &NodeStyle::statusPartial() const { return _statusPartial; }
ProcessingIconStyle const &NodeStyle::processingIconStyle() const { return _processingIconStyle; }

void NodeStyle::setStatusUpdated(QIcon const &icon) { _statusUpdated = icon; }
void NodeStyle::setStatusProcessing(QIcon const &icon) { _statusProcessing = icon; }
void NodeStyle::setStatusPending(QIcon const &icon) { _statusPending = icon; }
void NodeStyle::setStatusInvalid(QIcon const &icon) { _statusInvalid = icon; }
void NodeStyle::setStatusEmpty(QIcon const &icon) { _statusEmpty = icon; }
void NodeStyle::setStatusPartial(QIcon const &icon) { _statusPartial = icon; }
void NodeStyle::setProcessingIconStyle(ProcessingIconStyle const &style) { _processingIconStyle = style; }
