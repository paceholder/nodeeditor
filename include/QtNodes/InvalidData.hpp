#pragma once

#include <QtNodes/NodeData>

namespace QtNodes {
    class InvalidData final : public NodeData {
    public:
        InvalidData() = default;

        [[nodiscard]] NodeDataType type() const override {
            return NodeDataType{"invalid", "Invalid", {0, 0, 0}, {}};
        }

        [[nodiscard]] bool empty() const override { return true; }

        void allowConversionFrom(const QString &id) override {
            Q_UNUSED(id)
        }
    };
} // QtNodes
