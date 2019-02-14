/****************************************************************************
** Meta object code from reading C++ file 'klustersview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/klustersview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'klustersview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KlustersView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      44,       // signalCount

 // signals: signature, parameters, type, tag, flags
      36,   14,   13,   13, 0x05,
      80,   63,   13,   13, 0x05,
     126,  109,   13,   13, 0x05,
     159,   63,   13,   13, 0x05,
     218,  188,   13,   13, 0x05,
     262,   63,   13,   13, 0x05,
     314,  294,   13,   13, 0x05,
     371,  358,   13,   13, 0x05,
     426,  398,   13,   13, 0x05,
     457,   13,   13,   13, 0x05,
     474,   13,   13,   13, 0x05,
     536,  491,   13,   13, 0x05,
     600,  576,   13,   13, 0x25,
     635,  576,   13,   13, 0x05,
     695,  674,   13,   13, 0x05,
     723,   13,   13,   13, 0x05,
     736,   13,   13,   13, 0x05,
     752,   13,   13,   13, 0x05,
     771,   13,   13,   13, 0x05,
     798,   13,   13,   13, 0x05,
     820,   13,   13,   13, 0x05,
     845,   13,   13,   13, 0x05,
     865,   13,   13,   13, 0x05,
     894,  885,   13,   13, 0x05,
     922,   13,   13,   13, 0x05,
     956,   13,   13,   13, 0x05,
     990,   13,   13,   13, 0x05,
    1000,   13,   13,   13, 0x05,
    1011,   13,   13,   13, 0x05,
    1042, 1027,   13,   13, 0x05,
    1080, 1078,   13,   13, 0x05,
    1102,   13,   13,   13, 0x05,
    1134, 1118,   13,   13, 0x05,
    1162, 1150,   13,   13, 0x05,
    1201, 1191,   13,   13, 0x05,
    1237,   13,   13,   13, 0x05,
    1266, 1260,   13,   13, 0x05,
    1303, 1296,   13,   13, 0x05,
    1370, 1328,   13,   13, 0x05,
    1423,   13,   13,   13, 0x05,
    1446,   13,   13,   13, 0x05,
    1474, 1469,   13,   13, 0x05,
    1491,   13,   13,   13, 0x05,
    1505,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
    1527, 1523,   13,   13, 0x0a,
    1568, 1555,   13,   13, 0x0a,
    1613, 1597,   13,   13, 0x0a,
    1661, 1645,   13,   13, 0x0a,
    1705, 1693,   13,   13, 0x0a,
    1746, 1731,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KlustersView[] = {
    "KlustersView\0\0dimensionX,dimensionY\0"
    "updatedDimensions(int,int)\0clusterId,active\0"
    "singleColorUpdated(int,bool)\0"
    "ClusterId,active\0clusterRemovedFromView(int,bool)\0"
    "clusterAddedToView(int,bool)\0"
    "fromClusters,clusterId,active\0"
    "newClusterAddedToView(QList<int>&,int,bool)\0"
    "newClusterAddedToView(int,bool)\0"
    "fromClusters,active\0"
    "spikesRemovedFromClusters(QList<int>&,bool)\0"
    "selectedMode\0modeToSet(BaseFrame::Mode)\0"
    "destinationClusterId,active\0"
    "spikesAddedToCluster(int,bool)\0"
    "updateContents()\0emptySelection()\0"
    "modifiedClusters,active,isModifiedByDeletion\0"
    "modifiedClusters(QList<int>&,bool,bool)\0"
    "modifiedClusters,active\0"
    "modifiedClusters(QList<int>&,bool)\0"
    "modifiedClustersUndo(QList<int>&,bool)\0"
    "start,timeFrameWidth\0updatedTimeFrame(long,long)\0"
    "sampleMode()\0timeFrameMode()\0"
    "meanPresentation()\0allWaveformsPresentation()\0"
    "overLayPresentation()\0sideBySidePresentation()\0"
    "increaseAmplitude()\0decreaseAmplitude()\0"
    "nbSpikes\0updateDisplayNbSpikes(long)\0"
    "increaseAmplitudeofCorrelograms()\0"
    "decreaseAmplitudeofCorrelograms()\0"
    "noScale()\0maxScale()\0shoulderScale()\0"
    "size,timeFrame\0updatedBinSizeAndTimeFrame(int,int)\0"
    "b\0setShoulderLine(bool)\0updateDrawing()\0"
    "acquisitionGain\0changeGain(int)\0"
    "step,active\0changeTimeInterval(int,bool)\0"
    "positions\0changeChannelPositions(QList<int>&)\0"
    "computeProbabilities()\0color\0"
    "changeBackgroundColor(QColor)\0active\0"
    "clustersRenumbered(bool)\0"
    "name,clustersToShow,clustersColors,active\0"
    "updateClusters(QString,QList<int>&,ItemColors*,bool)\0"
    "increaseAllAmplitude()\0decreaseAllAmplitude()\0"
    "show\0showLabels(bool)\0nextCluster()\0"
    "previousCluster()\0obj\0clusterDockClosed(QObject*)\0"
    "waveformView\0waveformDockClosed(QObject*)\0"
    "correlogramView\0correlogramDockClosed(QObject*)\0"
    "errorMatrixView\0errorMatrixDockClosed(QObject*)\0"
    "traceWidget\0traceDockClosed(QObject*)\0"
    "start,duration\0setStartAndDuration(long,long)\0"
};

void KlustersView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KlustersView *_t = static_cast<KlustersView *>(_o);
        switch (_id) {
        case 0: _t->updatedDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->singleColorUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->clusterRemovedFromView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->clusterAddedToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->newClusterAddedToView((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->newClusterAddedToView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->spikesRemovedFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->modeToSet((*reinterpret_cast< BaseFrame::Mode(*)>(_a[1]))); break;
        case 8: _t->spikesAddedToCluster((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->updateContents(); break;
        case 10: _t->emptySelection(); break;
        case 11: _t->modifiedClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 12: _t->modifiedClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: _t->modifiedClustersUndo((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 14: _t->updatedTimeFrame((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 15: _t->sampleMode(); break;
        case 16: _t->timeFrameMode(); break;
        case 17: _t->meanPresentation(); break;
        case 18: _t->allWaveformsPresentation(); break;
        case 19: _t->overLayPresentation(); break;
        case 20: _t->sideBySidePresentation(); break;
        case 21: _t->increaseAmplitude(); break;
        case 22: _t->decreaseAmplitude(); break;
        case 23: _t->updateDisplayNbSpikes((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 24: _t->increaseAmplitudeofCorrelograms(); break;
        case 25: _t->decreaseAmplitudeofCorrelograms(); break;
        case 26: _t->noScale(); break;
        case 27: _t->maxScale(); break;
        case 28: _t->shoulderScale(); break;
        case 29: _t->updatedBinSizeAndTimeFrame((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 30: _t->setShoulderLine((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 31: _t->updateDrawing(); break;
        case 32: _t->changeGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->changeTimeInterval((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 34: _t->changeChannelPositions((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 35: _t->computeProbabilities(); break;
        case 36: _t->changeBackgroundColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 37: _t->clustersRenumbered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 38: _t->updateClusters((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2])),(*reinterpret_cast< ItemColors*(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 39: _t->increaseAllAmplitude(); break;
        case 40: _t->decreaseAllAmplitude(); break;
        case 41: _t->showLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 42: _t->nextCluster(); break;
        case 43: _t->previousCluster(); break;
        case 44: _t->clusterDockClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 45: _t->waveformDockClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 46: _t->correlogramDockClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 47: _t->errorMatrixDockClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 48: _t->traceDockClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 49: _t->setStartAndDuration((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KlustersView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KlustersView::staticMetaObject = {
    { &DockArea::staticMetaObject, qt_meta_stringdata_KlustersView,
      qt_meta_data_KlustersView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KlustersView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KlustersView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KlustersView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KlustersView))
        return static_cast<void*>(const_cast< KlustersView*>(this));
    return DockArea::qt_metacast(_clname);
}

int KlustersView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void KlustersView::updatedDimensions(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KlustersView::singleColorUpdated(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KlustersView::clusterRemovedFromView(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KlustersView::clusterAddedToView(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KlustersView::newClusterAddedToView(QList<int> & _t1, int _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KlustersView::newClusterAddedToView(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KlustersView::spikesRemovedFromClusters(QList<int> & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KlustersView::modeToSet(BaseFrame::Mode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KlustersView::spikesAddedToCluster(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void KlustersView::updateContents()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void KlustersView::emptySelection()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void KlustersView::modifiedClusters(QList<int> & _t1, bool _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 13
void KlustersView::modifiedClustersUndo(QList<int> & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void KlustersView::updatedTimeFrame(long _t1, long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void KlustersView::sampleMode()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void KlustersView::timeFrameMode()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void KlustersView::meanPresentation()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void KlustersView::allWaveformsPresentation()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}

// SIGNAL 19
void KlustersView::overLayPresentation()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void KlustersView::sideBySidePresentation()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void KlustersView::increaseAmplitude()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void KlustersView::decreaseAmplitude()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}

// SIGNAL 23
void KlustersView::updateDisplayNbSpikes(long _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void KlustersView::increaseAmplitudeofCorrelograms()
{
    QMetaObject::activate(this, &staticMetaObject, 24, 0);
}

// SIGNAL 25
void KlustersView::decreaseAmplitudeofCorrelograms()
{
    QMetaObject::activate(this, &staticMetaObject, 25, 0);
}

// SIGNAL 26
void KlustersView::noScale()
{
    QMetaObject::activate(this, &staticMetaObject, 26, 0);
}

// SIGNAL 27
void KlustersView::maxScale()
{
    QMetaObject::activate(this, &staticMetaObject, 27, 0);
}

// SIGNAL 28
void KlustersView::shoulderScale()
{
    QMetaObject::activate(this, &staticMetaObject, 28, 0);
}

// SIGNAL 29
void KlustersView::updatedBinSizeAndTimeFrame(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void KlustersView::setShoulderLine(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void KlustersView::updateDrawing()
{
    QMetaObject::activate(this, &staticMetaObject, 31, 0);
}

// SIGNAL 32
void KlustersView::changeGain(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void KlustersView::changeTimeInterval(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void KlustersView::changeChannelPositions(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void KlustersView::computeProbabilities()
{
    QMetaObject::activate(this, &staticMetaObject, 35, 0);
}

// SIGNAL 36
void KlustersView::changeBackgroundColor(QColor _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 36, _a);
}

// SIGNAL 37
void KlustersView::clustersRenumbered(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 37, _a);
}

// SIGNAL 38
void KlustersView::updateClusters(QString _t1, QList<int> & _t2, ItemColors * _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 38, _a);
}

// SIGNAL 39
void KlustersView::increaseAllAmplitude()
{
    QMetaObject::activate(this, &staticMetaObject, 39, 0);
}

// SIGNAL 40
void KlustersView::decreaseAllAmplitude()
{
    QMetaObject::activate(this, &staticMetaObject, 40, 0);
}

// SIGNAL 41
void KlustersView::showLabels(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 41, _a);
}

// SIGNAL 42
void KlustersView::nextCluster()
{
    QMetaObject::activate(this, &staticMetaObject, 42, 0);
}

// SIGNAL 43
void KlustersView::previousCluster()
{
    QMetaObject::activate(this, &staticMetaObject, 43, 0);
}
QT_END_MOC_NAMESPACE
