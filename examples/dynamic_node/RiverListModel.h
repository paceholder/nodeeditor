#pragma once

#include "RiverListData.h"
#include <nodes/NodeData>
#include <nodes/NodeDataModel>
#include <memory>
#include <QCoreApplication>


using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;


class RiverListModel : public NodeDataModel {
    Q_DECLARE_TR_FUNCTIONS( RiverListModel )


public:
    RiverListModel();

    virtual ~RiverListModel() override;


public:
    QString caption() const override {
        return tr( "RiverList" );
    }

    bool captionVisible() const override {
        return true;
    }

    QString name() const override {
        return "RiverList";
    }

    unsigned int nPorts( PortType ) const override;

    QString portCaption( PortType, PortIndex ) const override;

    bool portCaptionVisible( PortType, PortIndex ) const override {
        return true;
    }

    NodeDataType dataType( PortType, PortIndex ) const override;

    ConnectionPolicy portOutConnectionPolicy( PortIndex ) const override {
      return ConnectionPolicy::One;
    }

    std::shared_ptr< NodeData > outData( PortIndex ) override;

    void setInData( std::shared_ptr< NodeData >, PortIndex ) override;

    QWidget *embeddedWidget() override;


private:
  std::shared_ptr< RiverListData > data_;
};
