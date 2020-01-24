/****************************************************************************
** Meta object code from reading C++ file 'GraphModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/GraphModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__GraphModel_t {
    const uint offsetsAndSize[44];
    char stringdata0[305];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__GraphModel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__GraphModel_t qt_meta_stringdata_QtNodes__GraphModel = {
    {
QT_MOC_LITERAL(0, 19), // "QtNodes::GraphModel"
QT_MOC_LITERAL(20, 17), // "connectionCreated"
QT_MOC_LITERAL(38, 0), // ""
QT_MOC_LITERAL(39, 12), // "ConnectionId"
QT_MOC_LITERAL(52, 12), // "connectionId"
QT_MOC_LITERAL(65, 17), // "connectionDeleted"
QT_MOC_LITERAL(83, 11), // "nodeCreated"
QT_MOC_LITERAL(95, 6), // "NodeId"
QT_MOC_LITERAL(102, 6), // "nodeId"
QT_MOC_LITERAL(109, 11), // "nodeDeleted"
QT_MOC_LITERAL(121, 18), // "nodePositonUpdated"
QT_MOC_LITERAL(140, 11), // "portDataSet"
QT_MOC_LITERAL(152, 8), // "PortType"
QT_MOC_LITERAL(161, 8), // "portType"
QT_MOC_LITERAL(170, 9), // "PortIndex"
QT_MOC_LITERAL(180, 9), // "portIndex"
QT_MOC_LITERAL(190, 21), // "portsAboutToBeDeleted"
QT_MOC_LITERAL(212, 29), // "std::unordered_set<PortIndex>"
QT_MOC_LITERAL(242, 12), // "portIndexSet"
QT_MOC_LITERAL(255, 12), // "portsDeleted"
QT_MOC_LITERAL(268, 22), // "portsAboutToBeInserted"
QT_MOC_LITERAL(291, 13) // "portsInserted"

    },
    "QtNodes::GraphModel\0connectionCreated\0"
    "\0ConnectionId\0connectionId\0connectionDeleted\0"
    "nodeCreated\0NodeId\0nodeId\0nodeDeleted\0"
    "nodePositonUpdated\0portDataSet\0PortType\0"
    "portType\0PortIndex\0portIndex\0"
    "portsAboutToBeDeleted\0"
    "std::unordered_set<PortIndex>\0"
    "portIndexSet\0portsDeleted\0"
    "portsAboutToBeInserted\0portsInserted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__GraphModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       5,    1,   77,    2, 0x06,    3 /* Public */,
       6,    1,   80,    2, 0x06,    5 /* Public */,
       9,    1,   83,    2, 0x06,    7 /* Public */,
      10,    1,   86,    2, 0x06,    9 /* Public */,
      11,    3,   89,    2, 0x06,   11 /* Public */,
      16,    3,   96,    2, 0x06,   15 /* Public */,
      19,    3,  103,    2, 0x06,   19 /* Public */,
      20,    3,  110,    2, 0x06,   23 /* Public */,
      21,    3,  117,    2, 0x06,   27 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 12, 0x80000000 | 14,    8,   13,   15,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 12, 0x80000000 | 17,    8,   13,   18,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 12, 0x80000000 | 17,    8,   13,   18,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 12, 0x80000000 | 17,    8,   13,   18,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 12, 0x80000000 | 17,    8,   13,   18,

       0        // eod
};

void QtNodes::GraphModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GraphModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 1: _t->connectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 2: _t->nodeCreated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 3: _t->nodeDeleted((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 4: _t->nodePositonUpdated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 5: _t->portDataSet((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const PortIndex(*)>(_a[3]))); break;
        case 6: _t->portsAboutToBeDeleted((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const std::unordered_set<PortIndex>(*)>(_a[3]))); break;
        case 7: _t->portsDeleted((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const std::unordered_set<PortIndex>(*)>(_a[3]))); break;
        case 8: _t->portsAboutToBeInserted((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const std::unordered_set<PortIndex>(*)>(_a[3]))); break;
        case 9: _t->portsInserted((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const std::unordered_set<PortIndex>(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GraphModel::*)(ConnectionId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::connectionCreated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(ConnectionId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::connectionDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::nodeCreated)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::nodeDeleted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::nodePositonUpdated)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const , PortType const , PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::portDataSet)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const , PortType const , std::unordered_set<PortIndex> const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::portsAboutToBeDeleted)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const , PortType const , std::unordered_set<PortIndex> const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::portsDeleted)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const , PortType const , std::unordered_set<PortIndex> const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::portsAboutToBeInserted)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (GraphModel::*)(NodeId const , PortType const , std::unordered_set<PortIndex> const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphModel::portsInserted)) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject QtNodes::GraphModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__GraphModel.offsetsAndSize,
    qt_meta_data_QtNodes__GraphModel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__GraphModel_t
, QtPrivate::TypeAndForceComplete<GraphModel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<std::unordered_set<PortIndex> const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<std::unordered_set<PortIndex> const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<std::unordered_set<PortIndex> const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NodeId const, std::false_type>, QtPrivate::TypeAndForceComplete<PortType const, std::false_type>, QtPrivate::TypeAndForceComplete<std::unordered_set<PortIndex> const &, std::false_type>



>,
    nullptr
} };


const QMetaObject *QtNodes::GraphModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::GraphModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__GraphModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::GraphModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QtNodes::GraphModel::connectionCreated(ConnectionId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::GraphModel::connectionDeleted(ConnectionId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::GraphModel::nodeCreated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtNodes::GraphModel::nodeDeleted(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtNodes::GraphModel::nodePositonUpdated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QtNodes::GraphModel::portDataSet(NodeId const _t1, PortType const _t2, PortIndex const _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtNodes::GraphModel::portsAboutToBeDeleted(NodeId const _t1, PortType const _t2, std::unordered_set<PortIndex> const & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QtNodes::GraphModel::portsDeleted(NodeId const _t1, PortType const _t2, std::unordered_set<PortIndex> const & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QtNodes::GraphModel::portsAboutToBeInserted(NodeId const _t1, PortType const _t2, std::unordered_set<PortIndex> const & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QtNodes::GraphModel::portsInserted(NodeId const _t1, PortType const _t2, std::unordered_set<PortIndex> const & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
