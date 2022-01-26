
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
#include "NodeDataModel.hpp"

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
  {  

  }

  void AddNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
  }


  void
  setGraphicsObject(std::shared_ptr<GroupGraphicsObject> graphics) {
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
    bool collapsed = _groupGraphicsObject->isCollapsed();
    groupJson["collapsed"] = (int)collapsed;

    QJsonObject posObj;
    posObj["x"] = _groupGraphicsObject->pos().x();
    posObj["y"] = _groupGraphicsObject->pos().y();
    groupJson["position"] = posObj;    

    QJsonObject sizeObj;
    sizeObj["x"] = collapsed ? _groupGraphicsObject->getSavedSizeX() : _groupGraphicsObject->getSizeX();
    sizeObj["y"] = collapsed ? _groupGraphicsObject->getSavedSizeY() :  _groupGraphicsObject->getSizeY();
    groupJson["size"] = sizeObj;    

    QJsonObject colObj;
    colObj["r"] = _groupGraphicsObject->r;
    colObj["g"] = _groupGraphicsObject->g;
    colObj["b"] = _groupGraphicsObject->b;
    groupJson["color"] = colObj;    
    return groupJson;
  };

  void restore(QJsonObject const &json) override;

  void restoreAtPosition(QJsonObject const &json, QPointF position);

  std::shared_ptr<GroupGraphicsObject> _groupGraphicsObject;
  QString _name;
private:

    FlowScene & _scene;
    std::vector<std::shared_ptr<Node>> nodes;

    QUuid _id;
};
}