#include "QtNodes/DataColors.hpp"

QMap<QString, QColor> QtNodes::DataColors::colors;


void QtNodes::DataColors::registerColor(const DataInfo &info) {
    colors[info.getDataId()] = info.getDataColor();
}

QColor QtNodes::DataColors::getColor(const QString &name) {
    if (colors.contains(name)) {
        return colors[name];
    }
    return QColor{168, 105, 49};
}

QColor QtNodes::DataColors::getColor(const QtNodes::DataInfo &info) {
    return getColor(info.getDataId());
}
