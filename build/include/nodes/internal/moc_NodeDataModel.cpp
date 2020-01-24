/****************************************************************************
** Meta object code from reading C++ file 'NodeDataModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/NodeDataModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NodeDataModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__NodeDataModel_t {
    const uint offsetsAndSize[28];
    char stringdata0[236];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__NodeDataModel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__NodeDataModel_t qt_meta_stringdata_QtNodes__NodeDataModel = {
    {
QT_MOC_LITERAL(0, 22), // "QtNodes::NodeDataModel"
QT_MOC_LITERAL(23, 11), // "dataUpdated"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 9), // "PortIndex"
QT_MOC_LITERAL(46, 5), // "index"
QT_MOC_LITERAL(52, 15), // "dataInvalidated"
QT_MOC_LITERAL(68, 16), // "computingStarted"
QT_MOC_LITERAL(85, 17), // "computingFinished"
QT_MOC_LITERAL(103, 25), // "embeddedWidgetSizeUpdated"
QT_MOC_LITERAL(129, 22), // "inputConnectionCreated"
QT_MOC_LITERAL(152, 12), // "ConnectionId"
QT_MOC_LITERAL(165, 22), // "inputConnectionDeleted"
QT_MOC_LITERAL(188, 23), // "outputConnectionCreated"
QT_MOC_LITERAL(212, 23) // "outputConnectionDeleted"

    },
    "QtNodes::NodeDataModel\0dataUpdated\0\0"
    "PortIndex\0index\0dataInvalidated\0"
    "computingStarted\0computingFinished\0"
    "embeddedWidgetSizeUpdated\0"
    "inputConnectionCreated\0ConnectionId\0"
    "inputConnectionDeleted\0outputConnectionCreated\0"
    "outputConnectionDeleted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__NodeDataModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       5,    1,   71,    2, 0x06,    3 /* Public */,
       6,    0,   74,    2, 0x06,    5 /* Public */,
       7,    0,   75,    2, 0x06,    6 /* Public */,
       8,    0,   76,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    1,   77,    2, 0x0a,    8 /* Public */,
      11,    1,   80,    2, 0x0a,   10 /* Public */,
      12,    1,   83,    2, 0x0a,   12 /* Public */,
      13,    1,   86,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void, 0x80000000 | 10,    2,

       0        // eod
};

void QtNodes::NodeDataModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NodeDataModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataUpdated((*reinterpret_cast< const PortIndex(*)>(_a[1]))); break;
        case 1: _t->dataInvalidated((*reinterpret_cast< const PortIndex(*)>(_a[1]))); break;
        case 2: _t->computingStarted(); break;
        case 3: _t->computingFinished(); break;
        case 4: _t->embeddedWidgetSizeUpdated(); break;
        case 5: _t->inputConnectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 6: _t->inputConnectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 7: _t->outputConnectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 8: _t->outputConnectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NodeDataModel::*)(PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDataModel::dataUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NodeDataModel::*)(PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDataModel::dataInvalidated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NodeDataModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDataModel::computingStarted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NodeDataModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDataModel::computingFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NodeDataModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDataModel::embeddedWidgetSizeUpdated)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject QtNodes::NodeDataModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__NodeDataModel.offsetsAndSize,
    qt_meta_data_QtNodes__NodeDataModel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__NodeDataModel_t
, QtPrivate::TypeAndForceComplete<NodeDataModel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<PortIndex const, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<ConnectionId const &, std::false_type>


>,
    nullptr
} };


const QMetaObject *QtNodes::NodeDataModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::NodeDataModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__NodeDataModel.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::NodeDataModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QtNodes::NodeDataModel::dataUpdated(PortIndex const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::NodeDataModel::dataInvalidated(PortIndex const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::NodeDataModel::computingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QtNodes::NodeDataModel::computingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void QtNodes::NodeDataModel::embeddedWidgetSizeUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
