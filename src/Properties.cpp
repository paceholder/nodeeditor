#include "Properties.hpp"

//#include <QtCore/QVariantMap>

#include <iostream>

void
Properties::
put(QString const &name, QVariant const &v)
{
  _values.insert(name, v);

  //if (!result.second)
  //std::cout << "Properties already have a value with the name '"
  //<< name.toLocal8Bit().data()
  //<< "'"
  //<< std::endl;
}


