#include "RiverListData.h"


RiverListData::RiverListData() {
}




RiverListData::d_t &
RiverListData::data( int i ) {
    return *std::next( data_.begin(), i );
}




void
RiverListData::add( const d_t &d, int i ) {
    if ( i == -1 ) i = static_cast<int>( data_.size() );
    data_.emplace( data_.begin() + i, d );
}




void RiverListData::remove( int i ) {
    const auto it = std::next( data_.cbegin(), i );
    data_.erase( it );
}
