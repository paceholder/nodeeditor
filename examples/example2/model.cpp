#include "model.hpp"

// For some reason CMake could not generate moc-files correctly
// without having a cpp for an QObject from hpp.

TextSourceDataModel::
TextSourceDataModel()
  : _lineEdit(new QLineEdit("Default Text"))
{
  //
}
