#include "RiverListModel.h"
#include "RiverData.h"


RiverListModel::RiverListModel() :
    data_( new RiverListData )
{
    data_->addBack( "" );
}




RiverListModel::~RiverListModel() {
}




unsigned int
RiverListModel::nPorts( PortType portType ) const {

    switch ( portType ) {
        case PortType::In:
            return data_->data().size();

        case PortType::Out:
            return 1;
    }

    return 0;
}




QString
RiverListModel::portCaption( PortType portType, PortIndex portIndex ) const {

    if ( portType == PortType::In ) {
        const QString text = data_->data( portIndex );
        return QString::number( portIndex + 1 ) + ") " + text;
    }

    return "";
}




NodeDataType
RiverListModel::dataType( PortType portType, PortIndex ) const {

    switch ( portType ) {
        case PortType::In:
            return RiverData().type();

        case PortType::Out:
            return RiverListData().type();
    }

    return RiverData().type();
}




std::shared_ptr< NodeData >
RiverListModel::outData( PortIndex ) {
    return data_;
}




void
RiverListModel::setInData( std::shared_ptr< NodeData > nd, PortIndex portIndex ) {

    const auto v = std::dynamic_pointer_cast< RiverData >( nd );
    if ( !v ) {
        // data removed (disconnected)
        // \todo We have the problems when reassign connections. Not good.
        if ( portIndex < data_->data().size() ) {
            data_->remove( portIndex );
            emit portRemoved();
        }
        return;
    }

    // data added (connected)
    data_->data( portIndex ) = v->data();

    // always add one more river when last input port connected
    if ( portIndex == (data_->data().size() - 1) ) {
        data_->addBack( "" );
        emit portAdded();
    }
}




QWidget *
RiverListModel::embeddedWidget() {
    return nullptr;
}
