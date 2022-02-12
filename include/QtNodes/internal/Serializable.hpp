#pragma once

#include <QtCore/QJsonObject>

namespace QtNodes
{

class Serializable
{
public:

  virtual
  ~Serializable() = default;

  virtual
  QJsonObject
  save() const = 0;

  virtual void
  restore(QJsonObject const & /*p*/) {}
};
}
