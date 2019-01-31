#pragma once

#include "RiverData.h"
#include <nodes/NodeData>
#include <nodes/NodeDataModel>
#include <memory>
#include <QLineEdit>
#include <QCoreApplication>


using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;


class RiverModel : public NodeDataModel {
    Q_DECLARE_TR_FUNCTIONS( RiverModel )


public:
    RiverModel();

    virtual ~RiverModel() override;


public:
    QString caption() const override {
        return tr( "River" );
    }

    bool captionVisible() const override {
        return false;
    }

    QString name() const override {
        return "River";
    }

    unsigned int nPorts( PortType ) const override;

    NodeDataType dataType( PortType, PortIndex ) const override;

    ConnectionPolicy portOutConnectionPolicy( PortIndex ) const override {
      return ConnectionPolicy::One;
    }

    std::shared_ptr< NodeData > outData( PortIndex ) override;

    void setInData( std::shared_ptr< NodeData >, PortIndex ) override;

    QWidget *embeddedWidget() override;


private slots:
  void onTextEdited( const QString & );


private:
  std::shared_ptr< RiverData > data_;
  QLineEdit *lineEdit_;
};
