#ifndef NODE_DATA_MODEL_HPP
#define NODE_DATA_MODEL_HPP

class NodeDataModel
{
public:

  unsigned int nSlots(EndType end) const;

  DataType type(EndType end, int slot);

  void setInputData(std::shared_ptr<NodeData> nodeData);

  QWidget * embeddedWidget();



public signal:
  void
};

#endif //  NODE_DATA_MODEL_HPP
