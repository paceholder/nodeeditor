/****************************************************************************
** Meta object code from reading C++ file 'Definitions.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/Definitions.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Definitions.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes_t {
    const uint offsetsAndSize[50];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes_t qt_meta_stringdata_QtNodes = {
    {
QT_MOC_LITERAL(0, 7), // "QtNodes"
QT_MOC_LITERAL(8, 8), // "NodeRole"
QT_MOC_LITERAL(17, 4), // "Type"
QT_MOC_LITERAL(22, 8), // "Position"
QT_MOC_LITERAL(31, 4), // "Size"
QT_MOC_LITERAL(36, 14), // "CaptionVisible"
QT_MOC_LITERAL(51, 7), // "Caption"
QT_MOC_LITERAL(59, 5), // "Style"
QT_MOC_LITERAL(65, 15), // "NumberOfInPorts"
QT_MOC_LITERAL(81, 16), // "NumberOfOutPorts"
QT_MOC_LITERAL(98, 6), // "Widget"
QT_MOC_LITERAL(105, 9), // "NodeFlags"
QT_MOC_LITERAL(115, 8), // "NodeFlag"
QT_MOC_LITERAL(124, 7), // "NoFlags"
QT_MOC_LITERAL(132, 9), // "Resizable"
QT_MOC_LITERAL(142, 8), // "PortRole"
QT_MOC_LITERAL(151, 4), // "Data"
QT_MOC_LITERAL(156, 8), // "DataType"
QT_MOC_LITERAL(165, 16), // "ConnectionPolicy"
QT_MOC_LITERAL(182, 3), // "One"
QT_MOC_LITERAL(186, 4), // "Many"
QT_MOC_LITERAL(191, 8), // "PortType"
QT_MOC_LITERAL(200, 2), // "In"
QT_MOC_LITERAL(203, 3), // "Out"
QT_MOC_LITERAL(207, 4) // "None"

    },
    "QtNodes\0NodeRole\0Type\0Position\0Size\0"
    "CaptionVisible\0Caption\0Style\0"
    "NumberOfInPorts\0NumberOfOutPorts\0"
    "Widget\0NodeFlags\0NodeFlag\0NoFlags\0"
    "Resizable\0PortRole\0Data\0DataType\0"
    "ConnectionPolicy\0One\0Many\0PortType\0"
    "In\0Out\0None"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       5,   14, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x2,    9,   39,
      11,   12, 0x1,    2,   57,
      15,   15, 0x2,    5,   61,
      18,   18, 0x2,    2,   71,
      21,   21, 0x2,    3,   75,

 // enum data: key, value
       2, uint(QtNodes::NodeRole::Type),
       3, uint(QtNodes::NodeRole::Position),
       4, uint(QtNodes::NodeRole::Size),
       5, uint(QtNodes::NodeRole::CaptionVisible),
       6, uint(QtNodes::NodeRole::Caption),
       7, uint(QtNodes::NodeRole::Style),
       8, uint(QtNodes::NodeRole::NumberOfInPorts),
       9, uint(QtNodes::NodeRole::NumberOfOutPorts),
      10, uint(QtNodes::NodeRole::Widget),
      13, uint(QtNodes::NoFlags),
      14, uint(QtNodes::Resizable),
      16, uint(QtNodes::PortRole::Data),
      17, uint(QtNodes::PortRole::DataType),
      18, uint(QtNodes::PortRole::ConnectionPolicy),
       5, uint(QtNodes::PortRole::CaptionVisible),
       6, uint(QtNodes::PortRole::Caption),
      19, uint(QtNodes::ConnectionPolicy::One),
      20, uint(QtNodes::ConnectionPolicy::Many),
      22, uint(QtNodes::PortType::In),
      23, uint(QtNodes::PortType::Out),
      24, uint(QtNodes::PortType::None),

       0        // eod
};

const QMetaObject QtNodes::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_QtNodes.offsetsAndSize,
    qt_meta_data_QtNodes,
    nullptr,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes_t
, QtPrivate::TypeAndForceComplete<void, std::true_type>



>,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
