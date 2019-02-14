/****************************************************************************
** Meta object code from reading C++ file 'tracewidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/tracewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tracewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TraceWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      66,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   13,   12,   12, 0x05,
      75,   54,   12,   12, 0x05,
     151,  109,   12,   12, 0x05,
     226,  192,   12,   12, 0x05,
     299,  259,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     340,  334,   12,   12, 0x0a,
     375,  370,   12,   12, 0x0a,
     399,  394,   12,   12, 0x0a,
     425,  416,   12,   12, 0x0a,
     453,  447,   12,   12, 0x0a,
     490,  483,   12,   12, 0x0a,
     523,  513,   12,   12, 0x0a,
     549,   13,   12,   12, 0x0a,
     602,  582,   12,   12, 0x0a,
     647,  632,   12,   12, 0x0a,
     692,  672,   12,   12, 0x0a,
     744,  726,   12,   12, 0x0a,
     793,  776,   12,   12, 0x0a,
     844,  822,   12,   12, 0x0a,
     901,  881,   12,   12, 0x0a,
     951,  936,   12,   12, 0x0a,
     978,   12,   12,   12, 0x0a,
    1009,   12,   12,   12, 0x0a,
    1051, 1040,   12,   12, 0x0a,
    1097, 1040,   12,   12, 0x0a,
    1164, 1143,   12,   12, 0x0a,
    1182,   12,   12,   12, 0x0a,
    1198,   12,   12,   12, 0x0a,
    1222, 1215,   12,   12, 0x0a,
    1243,   13,   12,   12, 0x0a,
    1300, 1270,   12,   12, 0x0a,
    1345, 1328,   12,   12, 0x0a,
    1368,   12,   12,   12, 0x0a,
    1381,   12,   12,   12, 0x0a,
    1394, 1389,   12,   12, 0x0a,
    1425, 1411,   12,   12, 0x0a,
    1472, 1460,   12,   12, 0x0a,
    1506, 1499,   12,   12, 0x0a,
    1687, 1538,   12,   12, 0x0a,
    1831, 1819,   12,   12, 0x0a,
    1937, 1867,   12,   12, 0x0a,
    2019, 1819,   12,   12, 0x0a,
    2104, 2053,   12,   12, 0x0a,
    2142,   12,   12,   12, 0x0a,
    2160,   12,   12,   12, 0x0a,
    2182,   12,   12,   12, 0x0a,
    2198,   12,   12,   12, 0x0a,
    2218,  109,   12,   12, 0x0a,
    2263,  192,   12,   12, 0x0a,
    2333, 2300,   12,   12, 0x0a,
    2374,   12,   12,   12, 0x0a,
    2413, 2388,   12,   12, 0x0a,
    2477, 2447,   12,   12, 0x0a,
    2515,  259,   12,   12, 0x0a,
    2587, 2554,   12,   12, 0x0a,
    2664, 2626,   12,   12, 0x0a,
    2704, 1215,   12,   12, 0x0a,
    2759, 2728,   12,   12, 0x0a,
    2840, 2807,   12,   12, 0x0a,
    2931, 2890,   12,   12, 0x0a,
    3000, 2984,   12,   12, 0x0a,
    3035, 3029,   12,   12, 0x08,
    3067, 3029,   12,   12, 0x08,
    3099, 3029,   12,   12, 0x08,
    3135,   12,   12,   12, 0x08,
    3157,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TraceWidget[] = {
    "TraceWidget\0\0selectedIds\0"
    "channelsSelected(QList<int>)\0"
    "startTime,timeWindow\0"
    "updateStartAndDuration(long,long)\0"
    "providerName,selectedEventId,time,newTime\0"
    "eventModified(QString,int,double,double)\0"
    "providerName,selectedEventId,time\0"
    "eventRemoved(QString,int,double)\0"
    "providerName,addedEventDescription,time\0"
    "eventAdded(QString,QString,double)\0"
    "color\0changeBackgroundColor(QColor)\0"
    "grey\0setGreyScale(bool)\0time\0"
    "moveToTime(long)\0multiple\0"
    "setMultiColumns(bool)\0lines\0"
    "setClusterVerticalLines(bool)\0raster\0"
    "setClusterRaster(bool)\0waveforms\0"
    "setClusterWaveforms(bool)\0"
    "slotChannelsSelected(QList<int>)\0"
    "selectedMode,active\0setMode(BaseFrame::Mode,bool)\0"
    "channelsToShow\0showChannels(QList<int>)\0"
    "name,clustersToShow\0"
    "showClusters(QString,QList<int>&)\0"
    "name,eventsToShow\0showEvents(QString,QList<int>&)\0"
    "channelId,active\0channelColorUpdate(int,bool)\0"
    "name,clusterId,active\0"
    "clusterColorUpdate(QString,int,bool)\0"
    "name,eventId,active\0"
    "eventColorUpdate(QString,int,bool)\0"
    "groupId,active\0groupColorUpdate(int,bool)\0"
    "increaseAllChannelsAmplitude()\0"
    "decreaseAllChannelsAmplitude()\0"
    "channelIds\0increaseSelectedChannelsAmplitude(QList<int>)\0"
    "decreaseSelectedChannelsAmplitude(QList<int>)\0"
    "gain,acquisitionGain\0setGains(int,int)\0"
    "updateDrawing()\0updateContents()\0"
    "active\0groupsModified(bool)\0"
    "selectChannels(QList<int>)\0"
    "selectedChannelDefaultOffsets\0"
    "resetOffsets(QMap<int,int>)\0"
    "selectedChannels\0resetGains(QList<int>)\0"
    "drawTraces()\0reset()\0show\0showLabels(bool)\0"
    "time,duration\0slotSetStartAndDuration(long,long)\0"
    "show,active\0showCalibration(bool,bool)\0"
    "length\0samplingRateModified(long long)\0"
    "clustersProvider,name,clusterColors,active,clustersToShow,displayGroup"
    "sClusterFile,channelsSpikeGroups,nbSamplesBefore,nbSamplesAfter,cluste"
    "rsToSkip\0"
    "addClusterProvider(ClustersProvider*,QString,ItemColors*,bool,QList<in"
    "t>&,QMap<int,QList<int> >*,QMap<int,int>*,int,int,QList<int>)\0"
    "name,active\0removeClusterProvider(QString,bool)\0"
    "eventsProvider,name,eventColors,active,eventsToShow,eventsToNotBrowse\0"
    "addEventProvider(EventsProvider*,QString,ItemColors*,bool,QList<int>&,"
    "QList<int>)\0"
    "removeEventProvider(QString,bool)\0"
    "printPainter,width,height,filePath,whiteBackground\0"
    "print(QPainter&,int,int,QString,bool)\0"
    "showNextCluster()\0showPreviousCluster()\0"
    "showNextEvent()\0showPreviousEvent()\0"
    "slotEventModified(QString,int,double,double)\0"
    "slotEventRemoved(QString,int,double)\0"
    "active,providerName,time,newTime\0"
    "updateEvents(bool,QString,double,double)\0"
    "removeEvent()\0active,providerName,time\0"
    "updateEvents(bool,QString,double)\0"
    "providerName,eventDescription\0"
    "eventToAddProperties(QString,QString)\0"
    "slotEventAdded(QString,QString,double)\0"
    "providerName,eventsToShow,active\0"
    "updateEvents(QString,QList<int>&,bool)\0"
    "nbSamplesBefore,nbSamplesAfter,active\0"
    "updateWaveformInformation(int,int,bool)\0"
    "updateClusterData(bool)\0"
    "providerName,eventsToNotBrowse\0"
    "updateNoneBrowsingEventList(QString,QList<int>)\0"
    "providerName,clustersToNotBrowse\0"
    "updateNoneBrowsingClusterList(QString,QList<int>)\0"
    "name,clustersToShow,clusterColors,active\0"
    "updateClusters(QString,QList<int>&,ItemColors*,bool)\0"
    "skippedChannels\0updateSkipStatus(QList<int>)\0"
    "start\0slotStartMinuteTimeUpdated(int)\0"
    "slotStartSecondTimeUpdated(int)\0"
    "slotStartMilisecondTimeUpdated(int)\0"
    "slotDurationUpdated()\0slotScrollBarUpdated()\0"
};

void TraceWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TraceWidget *_t = static_cast<TraceWidget *>(_o);
        switch (_id) {
        case 0: _t->channelsSelected((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 1: _t->updateStartAndDuration((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 2: _t->eventModified((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 3: _t->eventRemoved((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 4: _t->eventAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 5: _t->changeBackgroundColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 6: _t->setGreyScale((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->moveToTime((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 8: _t->setMultiColumns((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setClusterVerticalLines((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->setClusterRaster((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->setClusterWaveforms((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->slotChannelsSelected((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 13: _t->setMode((*reinterpret_cast< BaseFrame::Mode(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 14: _t->showChannels((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 15: _t->showClusters((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 16: _t->showEvents((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 17: _t->channelColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 18: _t->clusterColorUpdate((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 19: _t->eventColorUpdate((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 20: _t->groupColorUpdate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 21: _t->increaseAllChannelsAmplitude(); break;
        case 22: _t->decreaseAllChannelsAmplitude(); break;
        case 23: _t->increaseSelectedChannelsAmplitude((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 24: _t->decreaseSelectedChannelsAmplitude((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 25: _t->setGains((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 26: _t->updateDrawing(); break;
        case 27: _t->updateContents(); break;
        case 28: _t->groupsModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: _t->selectChannels((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 30: _t->resetOffsets((*reinterpret_cast< const QMap<int,int>(*)>(_a[1]))); break;
        case 31: _t->resetGains((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 32: _t->drawTraces(); break;
        case 33: _t->reset(); break;
        case 34: _t->showLabels((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 35: _t->slotSetStartAndDuration((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 36: _t->showCalibration((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 37: _t->samplingRateModified((*reinterpret_cast< long long(*)>(_a[1]))); break;
        case 38: _t->addClusterProvider((*reinterpret_cast< ClustersProvider*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< ItemColors*(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< QList<int>(*)>(_a[5])),(*reinterpret_cast< QMap<int,QList<int> >*(*)>(_a[6])),(*reinterpret_cast< QMap<int,int>*(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8])),(*reinterpret_cast< int(*)>(_a[9])),(*reinterpret_cast< const QList<int>(*)>(_a[10]))); break;
        case 39: _t->removeClusterProvider((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 40: _t->addEventProvider((*reinterpret_cast< EventsProvider*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< ItemColors*(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< QList<int>(*)>(_a[5])),(*reinterpret_cast< const QList<int>(*)>(_a[6]))); break;
        case 41: _t->removeEventProvider((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 42: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5]))); break;
        case 43: _t->showNextCluster(); break;
        case 44: _t->showPreviousCluster(); break;
        case 45: _t->showNextEvent(); break;
        case 46: _t->showPreviousEvent(); break;
        case 47: _t->slotEventModified((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 48: _t->slotEventRemoved((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 49: _t->updateEvents((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 50: _t->removeEvent(); break;
        case 51: _t->updateEvents((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 52: _t->eventToAddProperties((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 53: _t->slotEventAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 54: _t->updateEvents((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 55: _t->updateWaveformInformation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 56: _t->updateClusterData((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->updateNoneBrowsingEventList((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<int>(*)>(_a[2]))); break;
        case 58: _t->updateNoneBrowsingClusterList((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<int>(*)>(_a[2]))); break;
        case 59: _t->updateClusters((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2])),(*reinterpret_cast< ItemColors*(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 60: _t->updateSkipStatus((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 61: _t->slotStartMinuteTimeUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 62: _t->slotStartSecondTimeUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 63: _t->slotStartMilisecondTimeUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 64: _t->slotDurationUpdated(); break;
        case 65: _t->slotScrollBarUpdated(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TraceWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TraceWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TraceWidget,
      qt_meta_data_TraceWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TraceWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TraceWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TraceWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TraceWidget))
        return static_cast<void*>(const_cast< TraceWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int TraceWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 66)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 66;
    }
    return _id;
}

// SIGNAL 0
void TraceWidget::channelsSelected(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TraceWidget::updateStartAndDuration(long _t1, long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TraceWidget::eventModified(QString _t1, int _t2, double _t3, double _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TraceWidget::eventRemoved(QString _t1, int _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TraceWidget::eventAdded(QString _t1, QString _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
