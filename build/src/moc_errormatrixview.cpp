/****************************************************************************
** Meta object code from reading C++ file 'errormatrixview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/errormatrixview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'errormatrixview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ErrorMatrixView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   16,   17,   16, 0x0a,
      41,   16,   16,   16, 0x0a,
      93,   64,   16,   16, 0x0a,
     161,  126,   16,   16, 0x0a,
     244,  194,   16,   16, 0x0a,
     337,  298,   16,   16, 0x0a,
     418,  382,   16,   16, 0x0a,
     483,  463,   16,   16, 0x0a,
     530,  513,   16,   16, 0x0a,
     572,  555,   16,   16, 0x0a,
     634,  604,   16,   16, 0x0a,
     698,  684,   16,   16, 0x0a,
     740,  724,   16,   16, 0x0a,
     770,  513,   16,   16, 0x0a,
     870,  802,   16,   16, 0x0a,
     967,  937,   16,   16, 0x0a,
    1058, 1005,   16,   16, 0x0a,
    1121, 1105,   16,   16, 0x0a,
    1189, 1147,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ErrorMatrixView[] = {
    "ErrorMatrixView\0\0bool\0isThreadsRunning()\0"
    "updateMatrixContents()\0"
    "groupedClusters,newClusterId\0"
    "clustersGrouped(QList<int>&,int)\0"
    "deletedClusters,destinationCluster\0"
    "clustersDeleted(QList<int>&,int)\0"
    "fromClusters,destinationClusterId,emptiedClusters\0"
    "removeSpikesFromClusters(QList<int>&,int,QList<int>&)\0"
    "fromClusters,clusterId,emptiedClusters\0"
    "newClusterAdded(QList<int>&,int,QList<int>&)\0"
    "fromToNewClusterIds,emptiedClusters\0"
    "newClustersAdded(QMap<int,int>&,QList<int>&)\0"
    "clustersToRecluster\0newClustersAdded(QList<int>&)\0"
    "clusterIdsOldNew\0renumber(QMap<int,int>&)\0"
    "clusterIdsNewOld\0undoRenumbering(QMap<int,int>&)\0"
    "addedClusters,updatedClusters\0"
    "undoAdditionModification(QList<int>&,QList<int>&)\0"
    "addedClusters\0undoAddition(QList<int>&)\0"
    "updatedClusters\0undoModification(QList<int>&)\0"
    "redoRenumbering(QMap<int,int>&)\0"
    "addedClusters,modifiedClusters,isModifiedByDeletion,deletedClusters\0"
    "redoAdditionModification(QList<int>&,QList<int>&,bool,QList<int>&)\0"
    "addedClusters,deletedClusters\0"
    "redoAddition(QList<int>&,QList<int>&)\0"
    "updatedClusters,isModifiedByDeletion,deletedClusters\0"
    "redoModification(QList<int>&,bool,QList<int>&)\0"
    "deletedClusters\0redoDeletion(QList<int>&)\0"
    "printPainter,width,height,whiteBackground\0"
    "print(QPainter&,int,int,bool)\0"
};

void ErrorMatrixView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ErrorMatrixView *_t = static_cast<ErrorMatrixView *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->isThreadsRunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: _t->updateMatrixContents(); break;
        case 2: _t->clustersGrouped((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->clustersDeleted((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->removeSpikesFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 5: _t->newClusterAdded((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 6: _t->newClustersAdded((*reinterpret_cast< QMap<int,int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 7: _t->newClustersAdded((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 8: _t->renumber((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 9: _t->undoRenumbering((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 10: _t->undoAdditionModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 11: _t->undoAddition((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 12: _t->undoModification((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 13: _t->redoRenumbering((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 14: _t->redoAdditionModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< QList<int>(*)>(_a[4]))); break;
        case 15: _t->redoAddition((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 16: _t->redoModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 17: _t->redoDeletion((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 18: _t->print((*reinterpret_cast< QPainter(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ErrorMatrixView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ErrorMatrixView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_ErrorMatrixView,
      qt_meta_data_ErrorMatrixView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ErrorMatrixView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ErrorMatrixView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ErrorMatrixView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ErrorMatrixView))
        return static_cast<void*>(const_cast< ErrorMatrixView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int ErrorMatrixView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
