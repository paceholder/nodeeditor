#pragma once

#include <QString>
#include <QColor>

namespace QtNodes {

    class NodeInfo {

    public:
        NodeInfo() {

        }

        NodeInfo(const QString &nodeName, const QString &nodeId, const QColor &nodeColor, const QString &iconPath)
                : nodeName_(nodeName),
                  nodeId_(nodeId),
                  nodeColor_(nodeColor),
                  iconPath_(iconPath) {
        }

        NodeInfo(const QString &nodeName, const QString &nodeId, const QString &category, const QColor &nodeColor,
                 const QString &iconPath)
                : nodeName_(nodeName),
                  nodeId_(nodeId),
                  category_(category),
                  nodeColor_(nodeColor),
                  iconPath_(iconPath) {
        }

        [[nodiscard]] const QString &getNodeName() const {
            return nodeName_;
        }

        [[nodiscard]] const QString &getNodeId() const {
            return nodeId_;
        }

        [[nodiscard]] const QColor &getNodeColor() const {
            return nodeColor_;
        }

        [[nodiscard]] const QString &getIconPath() const {
            return iconPath_;
        }

        [[nodiscard]] const QString &getCategory() const {
            return category_;
        }


    private:
        QString nodeName_;
        QString nodeId_;
        QString category_;
        QColor nodeColor_;
        QString iconPath_;
    };

} // nitro