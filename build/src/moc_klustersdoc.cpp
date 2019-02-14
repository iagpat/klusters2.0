/****************************************************************************
** Meta object code from reading C++ file 'klustersdoc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/klustersdoc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'klustersdoc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KlustersDoc[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   13,   12,   12, 0x05,
      38,   13,   12,   12, 0x05,
      85,   56,   12,   12, 0x05,
     153,  118,   12,   12, 0x05,
     236,  186,   12,   12, 0x05,
     329,  290,   12,   12, 0x05,
     410,  374,   12,   12, 0x05,
     472,  455,   12,   12, 0x05,
     514,  497,   12,   12, 0x05,
     576,  546,   12,   12, 0x05,
     640,  626,   12,   12, 0x05,
     682,  666,   12,   12, 0x05,
     712,  455,   12,   12, 0x05,
     812,  744,   12,   12, 0x05,
     909,  879,   12,   12, 0x05,
    1000,  947,   12,   12, 0x05,
    1063, 1047,   12,   12, 0x05,
    1109, 1089,   12,   12, 0x05,
    1139,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
    1162, 1155,   12,   12, 0x0a,
    1192,   12,   12,   12, 0x0a,
    1211,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KlustersDoc[] = {
    "KlustersDoc\0\0undoNb\0updateUndoNb(int)\0"
    "updateRedoNb(int)\0groupedClusters,newClusterId\0"
    "clustersGrouped(QList<int>&,int)\0"
    "deletedClusters,destinationCluster\0"
    "clustersDeleted(QList<int>&,int)\0"
    "fromClusters,destinationClusterId,emptiedClusters\0"
    "removeSpikesFromClusters(QList<int>&,int,QList<int>&)\0"
    "fromClusters,clusterId,emptiedClusters\0"
    "newClusterAdded(QList<int>&,int,QList<int>&)\0"
    "fromToNewClusterIds,emptiedClusters\0"
    "newClustersAdded(QMap<int,int>&,QList<int>&)\0"
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
    "clustersToRecluster\0newClustersAdded(QList<int>&)\0"
    "spikesDeleted()\0sender\0"
    "updateAllViews(KlustersView*)\0"
    "renumberClusters()\0launchAutoSave()\0"
};

void KlustersDoc::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KlustersDoc *_t = static_cast<KlustersDoc *>(_o);
        switch (_id) {
        case 0: _t->updateUndoNb((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateRedoNb((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->clustersGrouped((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->clustersDeleted((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->removeSpikesFromClusters((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 5: _t->newClusterAdded((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 6: _t->newClustersAdded((*reinterpret_cast< QMap<int,int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 7: _t->renumber((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 8: _t->undoRenumbering((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 9: _t->undoAdditionModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 10: _t->undoAddition((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 11: _t->undoModification((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 12: _t->redoRenumbering((*reinterpret_cast< QMap<int,int>(*)>(_a[1]))); break;
        case 13: _t->redoAdditionModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< QList<int>(*)>(_a[4]))); break;
        case 14: _t->redoAddition((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QList<int>(*)>(_a[2]))); break;
        case 15: _t->redoModification((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< QList<int>(*)>(_a[3]))); break;
        case 16: _t->redoDeletion((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 17: _t->newClustersAdded((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 18: _t->spikesDeleted(); break;
        case 19: _t->updateAllViews((*reinterpret_cast< KlustersView*(*)>(_a[1]))); break;
        case 20: _t->renumberClusters(); break;
        case 21: _t->launchAutoSave(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KlustersDoc::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KlustersDoc::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KlustersDoc,
      qt_meta_data_KlustersDoc, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KlustersDoc::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KlustersDoc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KlustersDoc::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KlustersDoc))
        return static_cast<void*>(const_cast< KlustersDoc*>(this));
    return QObject::qt_metacast(_clname);
}

int KlustersDoc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void KlustersDoc::updateUndoNb(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KlustersDoc::updateRedoNb(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KlustersDoc::clustersGrouped(QList<int> & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KlustersDoc::clustersDeleted(QList<int> & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KlustersDoc::removeSpikesFromClusters(QList<int> & _t1, int _t2, QList<int> & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KlustersDoc::newClusterAdded(QList<int> & _t1, int _t2, QList<int> & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KlustersDoc::newClustersAdded(QMap<int,int> & _t1, QList<int> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KlustersDoc::renumber(QMap<int,int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KlustersDoc::undoRenumbering(QMap<int,int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void KlustersDoc::undoAdditionModification(QList<int> & _t1, QList<int> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void KlustersDoc::undoAddition(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void KlustersDoc::undoModification(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void KlustersDoc::redoRenumbering(QMap<int,int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void KlustersDoc::redoAdditionModification(QList<int> & _t1, QList<int> & _t2, bool _t3, QList<int> & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void KlustersDoc::redoAddition(QList<int> & _t1, QList<int> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void KlustersDoc::redoModification(QList<int> & _t1, bool _t2, QList<int> & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void KlustersDoc::redoDeletion(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void KlustersDoc::newClustersAdded(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void KlustersDoc::spikesDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}
QT_END_MOC_NAMESPACE
