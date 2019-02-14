/****************************************************************************
** Meta object code from reading C++ file 'traceview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/traceview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'traceview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TraceView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   11,   10,   10, 0x05,
      66,   52,   10,   10, 0x05,
     139,   97,   10,   10, 0x05,
     214,  180,   10,   10, 0x05,
     287,  247,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     337,  322,   10,   10, 0x0a,
     406,  378,   10,   10, 0x0a,
     488,  455,   10,   10, 0x0a,
     619,  549,   10,   10, 0x0a,
     689,  549,   10,   10, 0x0a,
     809,  763,   10,   10, 0x0a,
     884,  763,   10,   10, 0x0a,
     993,  963,   10,   10, 0x0a,
    1047, 1031,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TraceView[] = {
    "TraceView\0\0selectedIds\0"
    "channelsSelected(QList<int>)\0time,duration\0"
    "setStartAndDuration(long,long)\0"
    "providerName,selectedEventId,time,newTime\0"
    "eventModified(QString,int,double,double)\0"
    "providerName,selectedEventId,time\0"
    "eventRemoved(QString,int,double)\0"
    "providerName,addedEventDescription,time\0"
    "eventAdded(QString,QString,double)\0"
    "data,initiator\0dataAvailable(Array<dataType>&,QObject*)\0"
    "data,initiator,providerName\0"
    "dataAvailable(Array<dataType>&,QObject*,QString)\0"
    "times,ids,initiator,providerName\0"
    "dataAvailable(Array<dataType>&,Array<int>&,QObject*,QString)\0"
    "data,initiator,providerName,startingTime,startingTimeInRecordingUnits\0"
    "nextClusterDataAvailable(Array<dataType>&,QObject*,QString,long,long)\0"
    "previousClusterDataAvailable(Array<dataType>&,QObject*,QString,long,lo"
    "ng)\0"
    "times,ids,initiator,providerName,startingTime\0"
    "nextEventDataAvailable(Array<dataType>&,Array<int>&,QObject*,QString,l"
    "ong)\0"
    "previousEventDataAvailable(Array<dataType>&,Array<int>&,QObject*,QStri"
    "ng,long)\0"
    "providerName,eventDescription\0"
    "eventToAddProperties(QString,QString)\0"
    "skippedChannels\0skipStatusChanged(QList<int>)\0"
};

void TraceView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TraceView *_t = static_cast<TraceView *>(_o);
        switch (_id) {
        case 0: _t->channelsSelected((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 1: _t->setStartAndDuration((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 2: _t->eventModified((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 3: _t->eventRemoved((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 4: _t->eventAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 5: _t->dataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2]))); break;
        case 6: _t->dataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 7: _t->dataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 8: _t->nextClusterDataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< long(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 9: _t->previousClusterDataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< long(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 10: _t->nextEventDataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 11: _t->previousEventDataAvailable((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 12: _t->eventToAddProperties((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 13: _t->skipStatusChanged((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TraceView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TraceView::staticMetaObject = {
    { &BaseFrame::staticMetaObject, qt_meta_stringdata_TraceView,
      qt_meta_data_TraceView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TraceView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TraceView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TraceView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TraceView))
        return static_cast<void*>(const_cast< TraceView*>(this));
    return BaseFrame::qt_metacast(_clname);
}

int TraceView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void TraceView::channelsSelected(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TraceView::setStartAndDuration(long _t1, long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TraceView::eventModified(QString _t1, int _t2, double _t3, double _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TraceView::eventRemoved(QString _t1, int _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TraceView::eventAdded(QString _t1, QString _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
