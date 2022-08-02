#pragma once

#include <utility>
#include <QtCore/QUuid>

//namespace QtNodes
//{
//Q_NAMESPACE


//struct Port
//{
  //PortType type;

  //PortIndex index;

  //Port()
    //: type(PortType::None)
    //, index(INVALID)
  //{}

  //Port(PortType t, PortIndex i)
    //: type(t)
    //, index(i)
  //{}

  //bool
  //indexIsValid() { return index != INVALID; }

  //bool
  //portTypeIsValid() { return type != PortType::None; }
//};

//using PortAddress = std::pair<QUuid, PortIndex>;

//inline
//PortType
//oppositePort(PortType port)
//{
  //PortType result = PortType::None;

  //switch (port)
  //{
    //case PortType::In:
      //result = PortType::Out;
      //break;

    //case PortType::Out:
      //result = PortType::In;
      //break;

    //default:
      //break;
  //}

  //return result;
//}
//}
