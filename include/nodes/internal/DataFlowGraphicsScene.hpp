#pragma once

#include "BasicGraphicsScene.hpp"
#include "DataFlowGraphModel.hpp"
#include "Export.hpp"


namespace QtNodes
{

class NODE_EDITOR_PUBLIC DataFlowGraphicsScene
  : public BasicGraphicsScene
{
  Q_OBJECT
public:

  DataFlowGraphicsScene(DataFlowGraphModel &graphModel,
                        QObject * parent = nullptr);

  ~DataFlowGraphicsScene() = default;

public:

  std::vector<NodeId>
  selectedNodes() const;

public:


  QMenu *
  createSceneMenu(QPointF const scenePos) override;


public Q_SLOTS:

  void
  save() const;

  void
  load();

  void
  onPortDataSet(NodeId const    nodeId,
                PortType const  portType,
                PortIndex const portIndex);


  //std::shared_ptr<Connection> restoreConnection(QJsonObject const & connectionJson);

  //Node & restoreNode(QJsonObject const & nodeJson)


  //void save() const;

  //void load();

  //QByteArray saveToMemory() const;

  //void loadFromMemory(const QByteArray & data);

private:

  DataFlowGraphModel &_graphModel;
};

}
