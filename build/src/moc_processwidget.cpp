/****************************************************************************
** Meta object code from reading C++ file 'processwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/processwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'processwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProcessWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,
      52,   14,   14,   14, 0x05,
      77,   14,   14,   14, 0x05,
      86,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     123,  111,  106,   14, 0x0a,
     149,   14,   14,   14, 0x0a,
     164,  159,   14,   14, 0x0a,
     190,  159,   14,   14, 0x0a,
     216,   15,   14,   14, 0x09,
     260,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ProcessWidget[] = {
    "ProcessWidget\0\0,\0finished(int,QProcess::ExitStatus)\0"
    "processOutputsFinished()\0hidden()\0"
    "processNotStarted()\0bool\0dir,command\0"
    "startJob(QString,QString)\0killJob()\0"
    "line\0insertStdoutLine(QString)\0"
    "insertStderrLine(QString)\0"
    "slotProcessExited(int,QProcess::ExitStatus)\0"
    "slotOutputTreatmentOver()\0"
};

void ProcessWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProcessWidget *_t = static_cast<ProcessWidget *>(_o);
        switch (_id) {
        case 0: _t->finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 1: _t->processOutputsFinished(); break;
        case 2: _t->hidden(); break;
        case 3: _t->processNotStarted(); break;
        case 4: { bool _r = _t->startJob((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: _t->killJob(); break;
        case 6: _t->insertStdoutLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->insertStderrLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->slotProcessExited((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 9: _t->slotOutputTreatmentOver(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProcessWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProcessWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_ProcessWidget,
      qt_meta_data_ProcessWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProcessWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProcessWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProcessWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProcessWidget))
        return static_cast<void*>(const_cast< ProcessWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int ProcessWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ProcessWidget::finished(int _t1, QProcess::ExitStatus _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProcessWidget::processOutputsFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ProcessWidget::hidden()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ProcessWidget::processNotStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
