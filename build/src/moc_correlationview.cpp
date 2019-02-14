/****************************************************************************
** Meta object code from reading C++ file 'correlationview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/correlationview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'correlationview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CorrelationView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   17,   16,   16, 0x0a,
      62,   17,   16,   16, 0x0a,
      89,   17,   16,   16, 0x0a,
     151,  121,   16,   16, 0x0a,
     193,   17,   16,   16, 0x0a,
     243,  223,   16,   16, 0x0a,
     287,   17,   16,   16, 0x0a,
     331,  318,   16,   16, 0x0a,
     356,   16,   16,   16, 0x0a,
     369,   16,   16,   16, 0x0a,
     387,   16,   16,   16, 0x0a,
     417,  406,   16,   16, 0x0a,
     450,   16,   16,   16, 0x0a,
     470,   16,   16,   16, 0x0a,
     495,   16,  490,   16, 0x0a,
     538,  514,   16,   16, 0x0a,
     620,  575,   16,   16, 0x0a,
     660,  658,   16,   16, 0x0a,
     682,   16,   16,   16, 0x0a,
     705,  698,   16,   16, 0x0a,
     772,  730,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CorrelationView[] = {
    "CorrelationView\0\0clusterId,active\0"
    "singleColorUpdate(int,bool)\0"
    "addClusterToView(int,bool)\0"
    "removeClusterFromView(int,bool)\0"
    "fromClusters,clusterId,active\0"
    "addNewClusterToView(QList<int>&,int,bool)\0"
    "addNewClusterToView(int,bool)\0"
    "fromClusters,active\0"
    "spikesRemovedFromClusters(QList<int>&,bool)\0"
    "spikesAddedToCluster(int,bool)\0"
    "selectedMode\0setMode(BaseFrame::Mode)\0"
    "setNoScale()\0setMaximumScale()\0"
    "setShoulderScale()\0size,width\0"
    "setBinSizeAndTimeWindow(int,int)\0"
    "increaseAmplitude()\0decreaseAmplitude()\0"
    "bool\0isThreadsRunning()\0modifiedClusters,active\0"
    "undoUpdateClusters(QList<int>&,bool)\0"
    "modifiedClusters,active,isModifiedByDeletion\0"
    "updateClusters(QList<int>&,bool,bool)\0"
    "b\0setShoulderLine(bool)\0updateDrawing()\0"
    "active\0clustersRenumbered(bool)\0"
    "printPainter,width,height,whiteBackground\0"
    "print(QPainter&,int,int,bool)\0"
};

void CorrelationView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CorrelationView *_t = static_cast<CorrelationView *>(_o);
        switch (_id) {
        case 0: _t->singleColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->addClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->removeClusterFromView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->addNewClusterToView((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: _t->addNewClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->spikesRemovedFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->spikesAddedToCluster((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->setMode((*reinterpret_cast< BaseFrame::Mode(*)>(_a[1]))); break;
        case 8: _t->setNoScale(); break;
        case 9: _t->setMaximumScale(); break;
        case 10: _t->setShoulderScale(); break;
        case 11: _t->setBinSizeAndTimeWindow((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->increaseAmplitude(); break;
        case 13: _t->decreaseAmplitude(); break;
        case 14: { bool _r = _t->isThreadsRunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: _t->undoUpdateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 16: _t->updateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 17: _t->setShoulderLine((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->updateDrawing(); break;
        case 19: _t->clustersRenumbered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CorrelationView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CorrelationView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_CorrelationView,
      qt_meta_data_CorrelationView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CorrelationView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CorrelationView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CorrelationView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CorrelationView))
        return static_cast<void*>(const_cast< CorrelationView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int CorrelationView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
