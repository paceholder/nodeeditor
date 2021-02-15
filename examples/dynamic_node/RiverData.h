#pragma once

#include <nodes/NodeData>


using QtNodes::NodeData;
using QtNodes::NodeDataType;


class RiverData : public NodeData {
public:
    RiverData() : data_( "" ) {
    }

    explicit RiverData( const QString &s ) : data_( s ) {
    }

    NodeDataType type() const override {
        return NodeDataType{ "River", "" };
    }

    const QString &data() const {
        return data_;
    }


private:
    QString data_;
};
