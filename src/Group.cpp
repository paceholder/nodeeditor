#include "Group.hpp"
#include "GroupGraphicsObject.hpp"
#include "FlowScene.hpp"

using QtNodes::Group;

QUuid
Group::
id() const
{
  return _id;
}

void 
Group::
SetName(QString _name) {
  this->_name = _name;
}

QString 
Group::
GetName() {
  return _name;
}


void 
Group::
restore(QJsonObject const &json){
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
  
  
  Group &groupRef = *(this);
  _scene.resolveGroups(groupRef);

  bool collapsed = (bool)json["collapsed"].toInt();
  if(collapsed)
  {
    _groupGraphicsObject->Collapse();
  }
}

void 
Group::
restoreAtPosition(QJsonObject const &json, QPointF position){
  _groupGraphicsObject->setPos(position);
  
  QJsonObject sizeJson = json["size"].toObject();
  _groupGraphicsObject->setSizeX(sizeJson["x"].toDouble());
  _groupGraphicsObject->setSizeY(sizeJson["y"].toDouble());


  QJsonObject colorJson = json["color"].toObject();
  _groupGraphicsObject->r = colorJson["r"].toInt();
  _groupGraphicsObject->g = colorJson["g"].toInt();
  _groupGraphicsObject->b = colorJson["b"].toInt();

  SetName(json["name"].toString());
  _groupGraphicsObject->nameLineEdit->setText(_name);
  
  // "Problem : when restoring the group, it doesn't see the nodes that are within it."
  
  Group &groupRef = *(this);
  _scene.resolveGroups(groupRef);

  bool collapsed = (bool)json["collapsed"].toInt();
  if(collapsed)
  {
    _groupGraphicsObject->Collapse();
  }
};


// GroupGraphicsObject& Group::groupGraphicsObject(){
//   return *_groupGraphicsObject.get();
// }

