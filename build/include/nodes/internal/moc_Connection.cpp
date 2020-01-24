/****************************************************************************
** Meta object code from reading C++ file 'Connection.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/nodes/internal/Connection.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Connection.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__Connection_t {
    const uint offsetsAndSize[16];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QtNodes__Connection_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QtNodes__Connection_t qt_meta_stringdata_QtNodes__Connection = {
    {
QT_MOC_LITERAL(0, 19), // "QtNodes::Connection"
QT_MOC_LITERAL(20, 19), // "connectionCompleted"
QT_MOC_LITERAL(40, 0), // ""
QT_MOC_LITERAL(41, 10), // "Connection"
QT_MOC_LITERAL(52, 24), // "connectionMadeIncomplete"
QT_MOC_LITERAL(77, 7), // "updated"
QT_MOC_LITERAL(85, 11), // "Connection&"
QT_MOC_LITERAL(97, 4) // "conn"

    },
    "QtNodes::Connection\0connectionCompleted\0"
    "\0Connection\0connectionMadeIncomplete\0"
    "updated\0Connection&\0conn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__Connection[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x106,    1 /* Public | MethodIsConst  */,
       4,    1,   35,    2, 0x106,    3 /* Public | MethodIsConst  */,
       5,    1,   38,    2, 0x106,    5 /* Public | MethodIsConst  */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void QtNodes::Connection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Connection *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectionCompleted((*reinterpret_cast< const Connection(*)>(_a[1]))); break;
        case 1: _t->connectionMadeIncomplete((*reinterpret_cast< const Connection(*)>(_a[1]))); break;
        case 2: _t->updated((*reinterpret_cast< Connection(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Connection::*)(Connection const & ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Connection::connectionCompleted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Connection::*)(Connection const & ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Connection::connectionMadeIncomplete)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Connection::*)(Connection & ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Connection::updated)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject QtNodes::Connection::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__Connection.offsetsAndSize,
    qt_meta_data_QtNodes__Connection,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QtNodes__Connection_t
, QtPrivate::TypeAndForceComplete<Connection, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Connection const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Connection const &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<Connection &, std::false_type>



>,
    nullptr
} };


const QMetaObject *QtNodes::Connection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::Connection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__Connection.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::Connection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void QtNodes::Connection::connectionCompleted(Connection const & _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< QtNodes::Connection *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::Connection::connectionMadeIncomplete(Connection const & _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< QtNodes::Connection *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::Connection::updated(Connection & _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< QtNodes::Connection *>(this), &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
