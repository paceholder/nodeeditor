#pragma once

#include <QString>
#include <QColor>
#include <QMap>
#include "DataInfo.hpp"

namespace QtNodes {
    class DataColors {

    public:
        static QColor getColor(const QString& name);
        static QColor getColor(const DataInfo& info);
        static void registerColor(const DataInfo& info);

    private:
        static QMap<QString, QColor> colors;

    };
} // nitro