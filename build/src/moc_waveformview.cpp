/****************************************************************************
** Meta object code from reading C++ file 'waveformview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/waveformview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'waveformview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WaveformView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   14,   13,   13, 0x0a,
      59,   14,   13,   13, 0x0a,
      86,   14,   13,   13, 0x0a,
     148,  118,   13,   13, 0x0a,
     190,   14,   13,   13, 0x0a,
     240,  220,   13,   13, 0x0a,
     284,   14,   13,   13, 0x0a,
     328,  315,   13,   13, 0x0a,
     398,  353,   13,   13, 0x0a,
     460,  436,   13,   13, 0x0a,
     497,   13,   13,   13, 0x0a,
     519,   13,   13,   13, 0x0a,
     549,   13,   13,   13, 0x0a,
     574,   13,   13,   13, 0x0a,
     602,   13,   13,   13, 0x0a,
     618,   13,   13,   13, 0x0a,
     649,  637,   13,   13, 0x0a,
     678,  673,   13,   13, 0x0a,
     691,   13,   13,   13, 0x0a,
     711,   13,   13,   13, 0x0a,
     740,  731,   13,   13, 0x0a,
     778,  765,   13,   13, 0x0a,
     806,   13,  801,   13, 0x0a,
     825,   13,   13,   13, 0x0a,
     851,  841,   13,   13, 0x0a,
     891,  884,   13,   13, 0x0a,
     958,  916,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WaveformView[] = {
    "WaveformView\0\0clusterId,active\0"
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
    "modifiedClusters,active,isModifiedByDeletion\0"
    "updateClusters(QList<int>&,bool,bool)\0"
    "modifiedClusters,active\0"
    "undoUpdateClusters(QList<int>&,bool)\0"
    "setMeanPresentation()\0"
    "setAllWaveformsPresentation()\0"
    "setOverLayPresentation()\0"
    "setSideBySidePresentation()\0setSampleMode()\0"
    "setTimeFrameMode()\0start,width\0"
    "setTimeFrame(long,long)\0gain\0setGain(int)\0"
    "increaseAmplitude()\0decreaseAmplitude()\0"
    "nbSpikes\0setDisplayNbSpikes(long)\0"
    "bchIteration\0setWaveformBatch(long)\0"
    "bool\0isThreadsRunning()\0updateDrawing()\0"
    "positions\0setChannelPositions(QList<int>&)\0"
    "active\0clustersRenumbered(bool)\0"
    "printPainter,width,height,whiteBackground\0"
    "print(QPainter&,int,int,bool)\0"
};

void WaveformView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WaveformView *_t = static_cast<WaveformView *>(_o);
        switch (_id) {
        case 0: _t->singleColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->addClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->removeClusterFromView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->addNewClusterToView((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: _t->addNewClusterToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->spikesRemovedFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->spikesAddedToCluster((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->setMode((*reinterpret_cast< BaseFrame::Mode(*)>(_a[1]))); break;
        case 8: _t->updateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 9: _t->undoUpdateClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 10: _t->setMeanPresentation(); break;
        case 11: _t->setAllWaveformsPresentation(); break;
        case 12: _t->setOverLayPresentation(); break;
        case 13: _t->setSideBySidePresentation(); break;
        case 14: _t->setSampleMode(); break;
        case 15: _t->setTimeFrameMode(); break;
        case 16: _t->setTimeFrame((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 17: _t->setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->increaseAmplitude(); break;
        case 19: _t->decreaseAmplitude(); break;
        case 20: _t->setDisplayNbSpikes((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 21: _t->setWaveformBatch((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 22: { bool _r = _t->isThreadsRunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 23: _t->updateDrawing(); break;
        case 24: _t->setChannelPositions((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 25: _t->clustersRenumbered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WaveformView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WaveformView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_WaveformView,
      qt_meta_data_WaveformView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WaveformView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WaveformView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WaveformView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WaveformView))
        return static_cast<void*>(const_cast< WaveformView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int WaveformView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
