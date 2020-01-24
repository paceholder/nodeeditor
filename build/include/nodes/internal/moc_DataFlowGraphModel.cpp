/****************************************************************************
** Meta object code from reading C++ file 'DataFlowGraphModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/DataFlowGraphModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataFlowGraphModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__DataFlowGraphModel_t {
    const uint offsetsAndSize[16];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__DataFlowGraphModel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__DataFlowGraphModel_t qt_meta_stringdata_QtNodes__DataFlowGraphModel = {
    {
QT_MOC_LITERAL(0, 27), // "QtNodes::DataFlowGraphModel"
QT_MOC_LITERAL(28, 17), // "onNodeDataUpdated"
QT_MOC_LITERAL(46, 0), // ""
QT_MOC_LITERAL(47, 6), // "NodeId"
QT_MOC_LITERAL(54, 6), // "nodeId"
QT_MOC_LITERAL(61, 9), // "PortIndex"
QT_MOC_LITERAL(71, 9), // "portIndex"
QT_MOC_LITERAL(81, 20) // "propagateEmptyDataTo"

    },
    "QtNodes::DataFlowGraphModel\0"
    "onNodeDataUpdated\0\0NodeId\0nodeId\0"
    "PortIndex\0portIndex\0propagateEmptyDataTo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__DataFlowGraphModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   26,    2, 0x08,    1 /* Private */,
       7,    2,   31,    2, 0x08,    4 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

       0        // eod
};

void QtNodes::DataFlowGraphModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataFlowGraphModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onNodeDataUpdated((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2]))); break;
        case 1: _t->propagateEmptyDataTo((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject QtNodes::DataFlowGraphModel::staticMetaObject = { {
    QMetaObject::SuperData::link<GraphModel::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__DataFlowGraphModel.offsetsAndSize,
    qt_meta_data_QtNodes__DataFlowGraphModel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__DataFlowGraphModel_t
, QtPrivate::TypeAndForceComplete<DataFlowGraphModel, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>


>,
    nullptr
} };


const QMetaObject *QtNodes::DataFlowGraphModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::DataFlowGraphModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__DataFlowGraphModel.stringdata0))
        return static_cast<void*>(this);
    return GraphModel::qt_metacast(_clname);
}

int QtNodes::DataFlowGraphModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
