#include <nodes/NodeData>
#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;

class StringData : public NodeData {
public:
  StringData(std::string str) : data(str) {}

  NodeDataType
  type() const override
  {
    return NodeDataType {"String",
                         "String"};
  }

  std::string data;
};

class Source : public NodeDataModel
{
  Q_OBJECT

public:

  virtual
  ~Source() {}

public:

  QString
  caption() const override
  {
    return QString("Data Source");
  }

  QString
  name() const override
  { return QString("Source"); }

public:

  unsigned int
  nPorts(PortType portType) const override
  {
    switch (portType) {
      case PortType::In: return 0;
      case PortType::Out: return 1;
    }
    Q_UNREACHABLE();
  }

  NodeDataType
  dataType(PortType portType,
           PortIndex portIndex) const override
  {
    Q_ASSERT(portType == PortType::Out);

    return NodeDataType { "String", "String" };
  }

  std::shared_ptr<NodeData>
  outData(PortIndex port) override
  {
    return std::make_shared<StringData>("Hello World!");
  }

  void
  setInData(std::shared_ptr<NodeData>, int) override
  {
    Q_UNREACHABLE();
  }

  QWidget *
  embeddedWidget() override { return nullptr; }
};


class Sink : public NodeDataModel
{
  Q_OBJECT

public:

  virtual
  ~Sink() {}

public:

  QString
  caption() const override
  {
    return QString("Data Sink");
  }

  QString
  name() const override
  { return QString("Sink"); }

public:

  unsigned int
  nPorts(PortType portType) const override
  {
    switch (portType) {
      case PortType::In: return 1;
      case PortType ::Out: return 0;
    }
    Q_UNREACHABLE();
  }

  NodeDataType
  dataType(PortType portType,
           PortIndex portIndex) const override
  {
    Q_ASSERT(portType == PortType::In);

    return NodeDataType { "String", "String" };
  }

  std::shared_ptr<NodeData>
  outData(PortIndex port) override
  {
    Q_UNREACHABLE();
  }

  void
  setInData(std::shared_ptr<NodeData> in, int) override
  {
    if (!in) { data = ""; }
    else { data = std::static_pointer_cast<StringData>(in)->data; }
  }

  QWidget *
  embeddedWidget() override { return nullptr; }

public:
  std::string data;
};

