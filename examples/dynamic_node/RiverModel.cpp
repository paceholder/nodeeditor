#include "RiverModel.h"


RiverModel::RiverModel() :
    data_( new RiverData() ),
    lineEdit_( new QLineEdit() )
{
  lineEdit_->setMaximumSize( lineEdit_->sizeHint() );
  connect( lineEdit_, &QLineEdit::textChanged,
          this, &RiverModel::onTextEdited);
}




RiverModel::~RiverModel() {
}




unsigned int
RiverModel::nPorts( PortType portType ) const {

    switch ( portType ) {
        case PortType::In:
            return 0;

        case PortType::Out:
            return 1;
    }

    return 0;
}




NodeDataType
RiverModel::dataType( PortType, PortIndex ) const {
    return RiverData().type();
}




std::shared_ptr< NodeData >
RiverModel::outData( PortIndex ) {
    return data_;
}




void
RiverModel::setInData( std::shared_ptr< NodeData >, PortIndex ) {
}




QWidget *
RiverModel::embeddedWidget() {
    return lineEdit_;
}




void
RiverModel::onTextEdited( const QString &s ) {

    data_.reset( new RiverData( s ) );
    constexpr PortIndex portIndex = 0;
    emit dataUpdated( portIndex );
}
