#pragma once

#include "Properties.hpp"

class Serializable
{
public:

  virtual void
  save(Properties & p) const = 0;

  virtual void
  restore(Properties const &p) {}
};
