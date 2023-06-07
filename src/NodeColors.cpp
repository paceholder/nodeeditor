

#include "QtNodes/NodeColors.hpp"

QMap<QString, QColor> QtNodes::NodeColors::colors;


void QtNodes::NodeColors::registerColor(const NodeInfo &info) {
    colors[info.getNodeId()] = info.getNodeColor();
}

QColor QtNodes::NodeColors::getColor(const QString &name) {
    if (colors.contains(name)) {
        return colors[name];
    }
    return QColor{168, 105, 49};
}

QColor QtNodes::NodeColors::getColor(const QtNodes::NodeInfo &info) {
    return getColor(info.getNodeId());
}
