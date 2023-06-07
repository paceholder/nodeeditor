#pragma once

#include <QString>
#include <QColor>
#include <QMap>
#include "NodeInfo.hpp"

namespace QtNodes {
    class NodeColors {

    public:
        static QColor getColor(const QString& name);
        static QColor getColor(const NodeInfo& info);
        static void registerColor(const NodeInfo& info);

    private:
        static QMap<QString, QColor> colors;

    };
} // nitro