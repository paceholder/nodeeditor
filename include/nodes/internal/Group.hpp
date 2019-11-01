
#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtCore/QJsonObject>

#include "PortType.hpp"

#include "Export.hpp"
#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeData.hpp"
#include "NodeGraphicsObject.hpp"
#include "GroupGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"

namespace QtNodes
{

class NODE_EDITOR_PUBLIC Group
    : public QObject
    , public Serializable
{
    Q_OBJECT
public: 

  Group(FlowScene &scene): _scene(scene){
    // GroupGraphicsObject groupGraphicsObject;
  }


  virtual
  ~Group(){};

public:

  QJsonObject save() const override{
    QJsonObject object;
    return object;
  };

  void restore(QJsonObject const &json) override{};

private:
    FlowScene & _scene;
    std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;

};
}