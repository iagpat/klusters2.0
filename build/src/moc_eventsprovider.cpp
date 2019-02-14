/****************************************************************************
** Meta object code from reading C++ file 'eventsprovider.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/eventsprovider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eventsprovider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventsProvider[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      49,   16,   15,   15, 0x05,
     152,  106,   15,   15, 0x05,
     223,  106,   15,   15, 0x05,
     363,  298,   15,   15, 0x05,
     519,  435,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_EventsProvider[] = {
    "EventsProvider\0\0times,ids,initiator,providerName\0"
    "dataReady(Array<dataType>&,Array<int>&,QObject*,QString)\0"
    "times,ids,initiator,providerName,startingTime\0"
    "nextEventDataReady(Array<dataType>&,Array<int>&,QObject*,QString,long)\0"
    "previousEventDataReady(Array<dataType>&,Array<int>&,QObject*,QString,l"
    "ong)\0"
    "providerName,oldNewEventIds,newOldEventIds,eventDescriptionAdded\0"
    "newEventDescriptionCreated(QString,QMap<int,int>,QMap<int,int>,QString"
    ")\0"
    "providerName,oldNewEventIds,newOldEventIds,eventIdToRemove,eventDescri"
    "ptionToRemove\0"
    "eventDescriptionRemoved(QString,QMap<int,int>,QMap<int,int>,int,QStrin"
    "g)\0"
};

void EventsProvider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EventsProvider *_t = static_cast<EventsProvider *>(_o);
        switch (_id) {
        case 0: _t->dataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 1: _t->nextEventDataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 2: _t->previousEventDataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< Array<int>(*)>(_a[2])),(*reinterpret_cast< QObject*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 3: _t->newEventDescriptionCreated((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QMap<int,int>(*)>(_a[2])),(*reinterpret_cast< QMap<int,int>(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 4: _t->eventDescriptionRemoved((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QMap<int,int>(*)>(_a[2])),(*reinterpret_cast< QMap<int,int>(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventsProvider::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventsProvider::staticMetaObject = {
    { &DataProvider::staticMetaObject, qt_meta_stringdata_EventsProvider,
      qt_meta_data_EventsProvider, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventsProvider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventsProvider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventsProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventsProvider))
        return static_cast<void*>(const_cast< EventsProvider*>(this));
    return DataProvider::qt_metacast(_clname);
}

int EventsProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataProvider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void EventsProvider::dataReady(Array<dataType> & _t1, Array<int> & _t2, QObject * _t3, QString _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EventsProvider::nextEventDataReady(Array<dataType> & _t1, Array<int> & _t2, QObject * _t3, QString _t4, long _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EventsProvider::previousEventDataReady(Array<dataType> & _t1, Array<int> & _t2, QObject * _t3, QString _t4, long _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EventsProvider::newEventDescriptionCreated(QString _t1, QMap<int,int> _t2, QMap<int,int> _t3, QString _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EventsProvider::eventDescriptionRemoved(QString _t1, QMap<int,int> _t2, QMap<int,int> _t3, int _t4, QString _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
