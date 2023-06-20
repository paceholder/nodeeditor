#pragma once

#include <QtNodes/NodeData>

namespace QtNodes {
    class InvalidData : public NodeData {
    public:
        InvalidData() = default;

        [[nodiscard]] NodeDataType type() const override {
            return NodeDataType{"invalid", "Invalid"};
        }

        [[nodiscard]] bool empty() const override { return true; }

    };
} // QtNodes
