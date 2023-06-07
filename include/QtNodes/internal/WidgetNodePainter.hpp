#pragma once

#include <QTreeWidgetItem>
#include <QMap>
#include "QtNodes/AbstractNodePainter"

namespace QtNodes {

    class WidgetNodePainter : public AbstractNodePainter {
    public:
        WidgetNodePainter();

        void paint(QPainter *painter, NodeGraphicsObject &ngo) const override;

        void drawNodeBackground(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawNodeBoundary(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo) const;

        void drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const;
    };


} // nitro
