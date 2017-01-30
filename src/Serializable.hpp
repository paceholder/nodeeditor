#pragma once

#include "Properties.hpp"

namespace QtNodes
{

class Serializable
{
public:

  virtual
  ~Serializable() = default;

  virtual void
  save(Properties & p) const = 0;

  virtual void
  restore(Properties const & /*p*/) {}
};
}
