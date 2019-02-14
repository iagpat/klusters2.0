/****************************************************************************
** Meta object code from reading C++ file 'clusterview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/clusterview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clusterview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClusterView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   40,   12,   12, 0x0a,
     106,   89,   12,   12, 0x0a,
     134,   89,   12,   12, 0x0a,
     161,   89,   12,   12, 0x0a,
     223,  193,   12,   12, 0x0a,
     265,   89,   12,   12, 0x0a,
     315,  295,   12,   12, 0x0a,
     359,   89,   12,   12, 0x0a,
     390,   12,   12,   12, 0x0a,
     420,  407,   12,   12, 0x0a,
     490,  445,   12,   12, 0x0a,
     552,  528,   12,   12, 0x0a,
     594,  589,   12,   12, 0x0a,
     631,  619,   12,   12, 0x0a,
     703,  661,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClusterView[] = {
    "ClusterView\0\0startTime\0moveToTime(long)\0"
    "dimensionX,dimensionY\0updatedDimensions(int,int)\0"
    "clusterId,active\0singleColorUpdate(int,bool)\0"
    "addClusterToView(int,bool)\0"
    "removeClusterFromView(int,bool)\0"
    "fromClusters,clusterId,active\0"
    "addNewClusterToView(QList<int>&,int,bool)\0"
    "addNewClusterToView(int,bool)\0"
    "fromClusters,active\0"
    "spikesRemovedFromClusters(QList<int>&,bool)\0"
    "spikesAddedToCluster(int,bool)\0"
    "emptySelection()\0selectedMode\0"
    "setMode(BaseFrame::Mode)\0"
    "modifiedClusters,active,isModifiedByDeletion\0"
    "updateClusters(QList<int>&,bool,bool)\0"
    "modifiedClusters,active\0"
    "undoUpdateClusters(QList<int>&,bool)\0"
    "step\0setTimeStepInSecond(int)\0step,active\0"
    "setTimeStepInSecond(int,bool)\0"
    "printPainter,width,height,whiteBackground\0"
    "print(QPainter&,int,int,bool)\0"
};

void ClusterView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ClusterView *_t = static_cast<ClusterView *>(_o);
        switch (_id) {
        case 0: _t->moveToTime((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 1: _t->updatedDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->singleColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->addClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->removeClusterFromView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->addNewClusterToView((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 6: _t->addNewClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->spikesRemovedFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 8: _t->spikesAddedToCluster((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->emptySelection(); break;
        case 10: _t->setMode((*reinterpret_cast< BaseFrame::Mode(*)>(_a[1]))); break;
        case 11: _t->updateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 12: _t->undoUpdateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: _t->setTimeStepInSecond((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->setTimeStepInSecond((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 15: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ClusterView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ClusterView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_ClusterView,
      qt_meta_data_ClusterView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClusterView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClusterView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClusterView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClusterView))
        return static_cast<void*>(const_cast< ClusterView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int ClusterView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ClusterView::moveToTime(long _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
