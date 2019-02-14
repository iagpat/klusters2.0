/****************************************************************************
** Meta object code from reading C++ file 'viewwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/viewwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   12,   11,   11, 0x0a,
      78,   61,   11,   11, 0x0a,
     106,   61,   11,   11, 0x0a,
     133,   61,   11,   11, 0x0a,
     195,  165,   11,   11, 0x0a,
     237,   61,   11,   11, 0x0a,
     287,  267,   11,   11, 0x0a,
     331,   61,   11,   11, 0x0a,
     362,   11,   11,   11, 0x0a,
     424,  379,   11,   11, 0x0a,
     486,  462,   11,   11, 0x0a,
     528,   11,  523,   11, 0x0a,
     589,  547,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ViewWidget[] = {
    "ViewWidget\0\0dimensionX,dimensionY\0"
    "updatedDimensions(int,int)\0clusterId,active\0"
    "singleColorUpdate(int,bool)\0"
    "addClusterToView(int,bool)\0"
    "removeClusterFromView(int,bool)\0"
    "fromClusters,clusterId,active\0"
    "addNewClusterToView(QList<int>&,int,bool)\0"
    "addNewClusterToView(int,bool)\0"
    "fromClusters,active\0"
    "spikesRemovedFromClusters(QList<int>&,bool)\0"
    "spikesAddedToCluster(int,bool)\0"
    "emptySelection()\0"
    "modifiedClusters,active,isModifiedByDeletion\0"
    "updateClusters(QList<int>&,bool,bool)\0"
    "modifiedClusters,active\0"
    "undoUpdateClusters(QList<int>&,bool)\0"
    "bool\0isThreadsRunning()\0"
    "printPainter,width,height,whiteBackground\0"
    "print(QPainter&,int,int,bool)\0"
};

void ViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewWidget *_t = static_cast<ViewWidget *>(_o);
        switch (_id) {
        case 0: _t->updatedDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->singleColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->addClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->removeClusterFromView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->addNewClusterToView((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->addNewClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->spikesRemovedFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->spikesAddedToCluster((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 8: _t->emptySelection(); break;
        case 9: _t->updateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 10: _t->undoUpdateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 11: { bool _r = _t->isThreadsRunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewWidget::staticMetaObject = {
    { &BaseFrame::staticMetaObject, qt_meta_stringdata_ViewWidget,
      qt_meta_data_ViewWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewWidget))
        return static_cast<void*>(const_cast< ViewWidget*>(this));
    return BaseFrame::qt_metacast(_clname);
}

int ViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
