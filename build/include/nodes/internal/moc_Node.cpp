/****************************************************************************
** Meta object code from reading C++ file 'Node.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/Node.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Node.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__Node_t {
    const uint offsetsAndSize[20];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__Node_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__Node_t qt_meta_stringdata_QtNodes__Node = {
    {
QT_MOC_LITERAL(0, 13), // "QtNodes::Node"
QT_MOC_LITERAL(14, 13), // "propagateData"
QT_MOC_LITERAL(28, 0), // ""
QT_MOC_LITERAL(29, 25), // "std::shared_ptr<NodeData>"
QT_MOC_LITERAL(55, 8), // "nodeData"
QT_MOC_LITERAL(64, 9), // "PortIndex"
QT_MOC_LITERAL(74, 11), // "inPortIndex"
QT_MOC_LITERAL(86, 13), // "onDataUpdated"
QT_MOC_LITERAL(100, 5), // "index"
QT_MOC_LITERAL(106, 17) // "onNodeSizeUpdated"

    },
    "QtNodes::Node\0propagateData\0\0"
    "std::shared_ptr<NodeData>\0nodeData\0"
    "PortIndex\0inPortIndex\0onDataUpdated\0"
    "index\0onNodeSizeUpdated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__Node[] = {

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
       1,    2,   32,    2, 0x10a,    1 /* Public | MethodIsConst  */,
       7,    1,   37,    2, 0x0a,    4 /* Public */,
       9,    0,   40,    2, 0x0a,    6 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 5,    8,
    QMetaType::Void,

       0        // eod
};

void QtNodes::Node::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Node *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->propagateData((*reinterpret_cast< std::shared_ptr<NodeData>(*)>(_a[1])),(*reinterpret_cast< PortIndex(*)>(_a[2]))); break;
        case 1: _t->onDataUpdated((*reinterpret_cast< PortIndex(*)>(_a[1]))); break;
        case 2: _t->onNodeSizeUpdated(); break;
        default: ;
        }
    }
}

const QMetaObject QtNodes::Node::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__Node.offsetsAndSize,
    qt_meta_data_QtNodes__Node,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__Node_t
, QtPrivate::TypeAndForceComplete<Node, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<std::shared_ptr<NodeData>, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *QtNodes::Node::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::Node::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__Node.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::Node::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
