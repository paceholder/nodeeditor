#ifndef FLOW_ITEM_INTERFACE
#define FLOW_ITEM_INTERFACE

class FlowItemInterface
{
public:
  virtual
  QString
  getNodeTitle() const = 0;

  virtual
  QStringList
  getNodeInSignatures() const = 0;

  virtual
  QStringList
  getNodeOutSignatures() const = 0;

  virtual
  QMap<id, QVariant>
  setData() = 0;

  virtual
  QVariant
  getData() const = 0;
};
#endif // FLOW_ITEM_INTERFACE
