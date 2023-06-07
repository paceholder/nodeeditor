#pragma once

namespace QtNodes {
    class InvalidData : public NodeData {
    public:
        InvalidData() = default;

        [[nodiscard]] NodeDataType type() const override {
            return NodeDataType{"invalid", "Invalid"};
        }

    };
} // QtNodes