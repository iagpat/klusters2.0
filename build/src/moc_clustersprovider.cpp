/****************************************************************************
** Meta object code from reading C++ file 'clustersprovider.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/clustersprovider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clustersprovider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClustersProvider[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      46,   18,   17,   17, 0x05,
     161,   91,   17,   17, 0x05,
     227,   91,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ClustersProvider[] = {
    "ClustersProvider\0\0data,initiator,providerName\0"
    "dataReady(Array<dataType>&,QObject*,QString)\0"
    "data,initiator,providerName,startingTime,startingTimeInRecordingUnits\0"
    "nextClusterDataReady(Array<dataType>&,QObject*,QString,long,long)\0"
    "previousClusterDataReady(Array<dataType>&,QObject*,QString,long,long)\0"
};

void ClustersProvider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ClustersProvider *_t = static_cast<ClustersProvider *>(_o);
        switch (_id) {
        case 0: _t->dataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: _t->nextClusterDataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< long(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        case 2: _t->previousClusterDataReady((*reinterpret_cast< Array<dataType>(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< long(*)>(_a[4])),(*reinterpret_cast< long(*)>(_a[5]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ClustersProvider::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ClustersProvider::staticMetaObject = {
    { &DataProvider::staticMetaObject, qt_meta_stringdata_ClustersProvider,
      qt_meta_data_ClustersProvider, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClustersProvider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClustersProvider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClustersProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClustersProvider))
        return static_cast<void*>(const_cast< ClustersProvider*>(this));
    return DataProvider::qt_metacast(_clname);
}

int ClustersProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataProvider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ClustersProvider::dataReady(Array<dataType> & _t1, QObject * _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClustersProvider::nextClusterDataReady(Array<dataType> & _t1, QObject * _t2, QString _t3, long _t4, long _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClustersProvider::previousClusterDataReady(Array<dataType> & _t1, QObject * _t2, QString _t3, long _t4, long _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
