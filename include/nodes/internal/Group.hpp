
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
#include "GroupGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"
#include "Node.hpp"
namespace QtNodes
{

class GroupGraphicsObject;

class NODE_EDITOR_PUBLIC Group
    : public QObject
    , public Serializable
{
    Q_OBJECT
public: 
  GroupGraphicsObject &
  groupGraphicsObject() {
      return *_groupGraphicsObject.get();
  }
  
  GroupGraphicsObject const &
  groupGraphicsObject() const {
      return *_groupGraphicsObject.get();
  }

  Group(FlowScene &scene): _scene(scene),
                          _id(QUuid::createUuid()),
                          _groupGraphicsObject(nullptr),
                          _name("New Group")
  {  }

  void AddNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
  }


  void
  setGraphicsObject(std::shared_ptr<GroupGraphicsObject> graphics) {
    // _groupGraphicsObject = std::move(graphics);
    _groupGraphicsObject = graphics;
  }


  virtual
  ~Group(){};

  void SetName(QString _name);
  QString GetName();

public:


  QUuid
  id() const;

  QJsonObject save() const override{
    QJsonObject groupJson;
    groupJson["name"] = _name;

    QJsonObject posObj;
    posObj["x"] = _groupGraphicsObject->pos().x();
    posObj["y"] = _groupGraphicsObject->pos().y();
    groupJson["position"] = posObj;    

    QJsonObject sizeObj;
    sizeObj["x"] = _groupGraphicsObject->getSizeX();
    sizeObj["y"] = _groupGraphicsObject->getSizeY();
    groupJson["size"] = sizeObj;    

    QJsonObject colObj;
    colObj["r"] = _groupGraphicsObject->r;
    colObj["g"] = _groupGraphicsObject->g;
    colObj["b"] = _groupGraphicsObject->b;
    groupJson["color"] = colObj;    
    return groupJson;
  };

  void restore(QJsonObject const &json) override{
    QJsonObject positionJson = json["position"].toObject();
    QPointF     point(positionJson["x"].toDouble(),
                      positionJson["y"].toDouble());
    _groupGraphicsObject->setPos(point);
    
    QJsonObject sizeJson = json["size"].toObject();
    _groupGraphicsObject->setSizeX(sizeJson["x"].toDouble());
    _groupGraphicsObject->setSizeY(sizeJson["y"].toDouble());


    QJsonObject colorJson = json["color"].toObject();
    _groupGraphicsObject->r = colorJson["r"].toInt();
    _groupGraphicsObject->g = colorJson["g"].toInt();
    _groupGraphicsObject->b = colorJson["b"].toInt();

    SetName(json["name"].toString());
    _groupGraphicsObject->nameLineEdit->setText(_name);
  };

  std::shared_ptr<GroupGraphicsObject> _groupGraphicsObject;
  QString _name;
private:

    FlowScene & _scene;
    std::vector<std::shared_ptr<Node>> nodes;

    QUuid _id;
};
}