#include "Group.hpp"
#include "GroupGraphicsObject.hpp"

using QtNodes::Group;

QUuid
Group::
id() const
{
  return _id;
}

void 
Group::
SetName(QString _name) {
  this->_name = _name;
}

QString 
Group::
GetName() {
  return _name;
}


// GroupGraphicsObject& Group::groupGraphicsObject(){
//   return *_groupGraphicsObject.get();
// }

