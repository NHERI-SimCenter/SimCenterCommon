/****************************************************************************
** Meta object code from reading C++ file 'RandomVariableInputWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../RandomVariables/RandomVariableInputWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RandomVariableInputWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RandomVariableInputWidget_t {
    QByteArrayData data[4];
    char stringdata[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RandomVariableInputWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RandomVariableInputWidget_t qt_meta_stringdata_RandomVariableInputWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "RandomVariableInputWidget"
QT_MOC_LITERAL(1, 26, 17), // "addRandomVariable"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 5) // "clear"

    },
    "RandomVariableInputWidget\0addRandomVariable\0"
    "\0clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RandomVariableInputWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RandomVariableInputWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RandomVariableInputWidget *_t = static_cast<RandomVariableInputWidget *>(_o);
        switch (_id) {
        case 0: _t->addRandomVariable(); break;
        case 1: _t->clear(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject RandomVariableInputWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RandomVariableInputWidget.data,
      qt_meta_data_RandomVariableInputWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RandomVariableInputWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RandomVariableInputWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RandomVariableInputWidget.stringdata))
        return static_cast<void*>(const_cast< RandomVariableInputWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int RandomVariableInputWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
