#include "RiverListData.h"


RiverListData::RiverListData() {
}




RiverListData::d_t &
RiverListData::data( int i ) {
    return *std::next( data_.begin(), i );
}




void
RiverListData::addBack( const d_t &d ) {
    data_.emplace_back( d );
}




void RiverListData::remove( int i ) {
    const auto it = std::next( data_.cbegin(), i );
    data_.erase( it );
}
