/****************************************************************************
** Meta object code from reading C++ file 'DataFlowGraphicsScene.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/DataFlowGraphicsScene.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataFlowGraphicsScene.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t {
    const uint offsetsAndSize[22];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t qt_meta_stringdata_QtNodes__DataFlowGraphicsScene = {
    {
QT_MOC_LITERAL(0, 30), // "QtNodes::DataFlowGraphicsScene"
QT_MOC_LITERAL(31, 4), // "save"
QT_MOC_LITERAL(36, 0), // ""
QT_MOC_LITERAL(37, 4), // "load"
QT_MOC_LITERAL(42, 13), // "onPortDataSet"
QT_MOC_LITERAL(56, 6), // "NodeId"
QT_MOC_LITERAL(63, 6), // "nodeId"
QT_MOC_LITERAL(70, 8), // "PortType"
QT_MOC_LITERAL(79, 8), // "portType"
QT_MOC_LITERAL(88, 9), // "PortIndex"
QT_MOC_LITERAL(98, 9) // "portIndex"

    },
    "QtNodes::DataFlowGraphicsScene\0save\0"
    "\0load\0onPortDataSet\0NodeId\0nodeId\0"
    "PortType\0portType\0PortIndex\0portIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__DataFlowGraphicsScene[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x10a,    1 /* Public | MethodIsConst  */,
       3,    0,   33,    2, 0x0a,    2 /* Public */,
       4,    3,   34,    2, 0x0a,    3 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7, 0x80000000 | 9,    6,    8,   10,

       0        // eod
};

void QtNodes::DataFlowGraphicsScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataFlowGraphicsScene *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->save(); break;
        case 1: _t->load(); break;
        case 2: _t->onPortDataSet((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const PortIndex(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject QtNodes::DataFlowGraphicsScene::staticMetaObject = { {
    QMetaObject::SuperData::link<BasicGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__DataFlowGraphicsScene.offsetsAndSize,
    qt_meta_data_QtNodes__DataFlowGraphicsScene,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t
, QtPrivate::TypeAndForceComplete<DataFlowGraphicsScene, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>


>,
    nullptr
} };


const QMetaObject *QtNodes::DataFlowGraphicsScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::DataFlowGraphicsScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__DataFlowGraphicsScene.stringdata0))
        return static_cast<void*>(this);
    return BasicGraphicsScene::qt_metacast(_clname);
}

int QtNodes::DataFlowGraphicsScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BasicGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
