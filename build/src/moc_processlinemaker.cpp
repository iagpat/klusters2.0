/****************************************************************************
** Meta object code from reading C++ file 'processlinemaker.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/processlinemaker.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'processlinemaker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProcessLineMaker[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   18,   17,   17, 0x05,
      51,   18,   17,   17, 0x05,
      79,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     101,   17,   17,   17, 0x0a,
     122,   17,   17,   17, 0x0a,
     143,   17,   17,   17, 0x0a,
     162,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProcessLineMaker[] = {
    "ProcessLineMaker\0\0line\0"
    "receivedStdoutLine(QString)\0"
    "receivedStderrLine(QString)\0"
    "outputTreatmentOver()\0slotReceivedStdout()\0"
    "slotReceivedStderr()\0slotWidgetHidden()\0"
    "slotProcessExited()\0"
};

void ProcessLineMaker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProcessLineMaker *_t = static_cast<ProcessLineMaker *>(_o);
        switch (_id) {
        case 0: _t->receivedStdoutLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->receivedStderrLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->outputTreatmentOver(); break;
        case 3: _t->slotReceivedStdout(); break;
        case 4: _t->slotReceivedStderr(); break;
        case 5: _t->slotWidgetHidden(); break;
        case 6: _t->slotProcessExited(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProcessLineMaker::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProcessLineMaker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ProcessLineMaker,
      qt_meta_data_ProcessLineMaker, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProcessLineMaker::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProcessLineMaker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProcessLineMaker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProcessLineMaker))
        return static_cast<void*>(const_cast< ProcessLineMaker*>(this));
    return QObject::qt_metacast(_clname);
}

int ProcessLineMaker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ProcessLineMaker::receivedStdoutLine(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProcessLineMaker::receivedStderrLine(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProcessLineMaker::outputTreatmentOver()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
