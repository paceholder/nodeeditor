#pragma once

#include <nodes/NodeData>
#include <QCoreApplication>


using QtNodes::NodeData;
using QtNodes::NodeDataType;


class RiverListData : public NodeData {
    Q_DECLARE_TR_FUNCTIONS( RiverListData )

    typedef QString d_t;
    typedef std::vector< d_t > data_t;


public:
    RiverListData();

    NodeDataType type() const override {
        const QString name = tr( "RiverList" );
        return NodeDataType{ "RiverList", name };
    }

    const data_t &data() const {
        return data_;
    }

    d_t &data( int i );

    void addBack( const d_t & );

    void remove( int i );


private:
    data_t data_;
};
