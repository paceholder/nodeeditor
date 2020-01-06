
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
#include "Node.hpp"
namespace QtNodes
{

class NODE_EDITOR_PUBLIC Group
    : public QObject
    , public Serializable
{
    Q_OBJECT
public: 

  Group(FlowScene &scene): _scene(scene),
                          _id(QUuid::createUuid())
  {
    _groupGraphicsObject = std::make_unique<GroupGraphicsObject>(scene);
  }

  void AddNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
  }


  virtual
  ~Group(){};

public:


  QUuid
  id() const;

  QJsonObject save() const override{
    QJsonObject object;
    return object;
  };

  void restore(QJsonObject const &json) override{};

private:

    FlowScene & _scene;
    std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;
    std::vector<std::shared_ptr<Node>> nodes;

    QUuid _id;
};
}