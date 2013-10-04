#ifndef FLOW_ITEM_INTERFACE
#define FLOW_ITEM_INTERFACE

class FlowItemInterface
{
public:
  /** Returns the type of curren flow item */
  virtual
  QString
  getFlowItemSignature() const = 0;

  /** Caption of the block  */
  virtual
  QString
  getNodeTitle() const = 0;

  /** Signatures (types) of input node connections */
  virtual
  QStringList
  getNodeInSignatures() const = 0;

  /** Names (text to be printed) of input node connections */
  virtual
  QStringList
  getNodeInNames() const = 0;

  /** Signatures (types) of output node connections */
  virtual
  QStringList
  getNodeOutSignatures() const = 0;

  /** Names (text to be printed) of output node connections */
  virtual
  QStringList
  getNodeOutNames() const = 0;

  virtual
  QMap<id, QVariant>
  setData() = 0;

  virtual
  QVariant
  getData() const = 0;

public:
  /** clone constructor */
  virtual
  FlowItemInterface*
  clone() const = 0;
};
#endif // FLOW_ITEM_INTERFACE
