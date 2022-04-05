#pragma once

#include <nodes/NodeData>
#include <QCoreApplication>
#include <memory>


using QtNodes::NodeData;
using QtNodes::NodeDataType;

class RiverData;
class RiverListData : public NodeData {
    Q_DECLARE_TR_FUNCTIONS( RiverListData )

    typedef std::shared_ptr<RiverData> d_t;
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

    void add( const d_t &, int i = -1 );

    void remove( int i );


private:
    data_t data_;
};
