/****************************************************************************
** Meta object code from reading C++ file 'clusterPalette.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/clusterPalette.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clusterPalette.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClusterPaletteWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   22,   21,   21, 0x05,
      57,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ClusterPaletteWidget[] = {
    "ClusterPaletteWidget\0\0item\0"
    "changeColor(QListWidgetItem*)\0"
    "onItem(QListWidgetItem*)\0"
};

void ClusterPaletteWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ClusterPaletteWidget *_t = static_cast<ClusterPaletteWidget *>(_o);
        switch (_id) {
        case 0: _t->changeColor((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->onItem((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ClusterPaletteWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ClusterPaletteWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_ClusterPaletteWidget,
      qt_meta_data_ClusterPaletteWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClusterPaletteWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClusterPaletteWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClusterPaletteWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClusterPaletteWidget))
        return static_cast<void*>(const_cast< ClusterPaletteWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int ClusterPaletteWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ClusterPaletteWidget::changeColor(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClusterPaletteWidget::onItem(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_ClusterPalette[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   16,   15,   15, 0x05,
      72,   55,   15,   15, 0x05,
     104,   55,   15,   15, 0x05,
     130,   55,   15,   15, 0x05,
     162,   55,   15,   15, 0x05,
     197,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     231,  226,   15,   15, 0x0a,
     261,   15,   15,   15, 0x0a,
     283,   15,   15,   15, 0x0a,
     308,   15,   15,   15, 0x0a,
     324,   15,   15,   15, 0x0a,
     341,   15,   15,   15, 0x09,
     359,   15,   15,   15, 0x09,
     376,  226,   15,   15, 0x09,
     405,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ClusterPalette[] = {
    "ClusterPalette\0\0selectedCluster\0"
    "singleChangeColor(int)\0selectedClusters\0"
    "updateShownClusters(QList<int>)\0"
    "groupClusters(QList<int>)\0"
    "moveClustersToNoise(QList<int>)\0"
    "moveClustersToArtefact(QList<int>)\0"
    "clusterInformationModified()\0item\0"
    "changeColor(QListWidgetItem*)\0"
    "moveClustersToNoise()\0moveClustersToArtefact()\0"
    "groupClusters()\0updateClusters()\0"
    "slotClickRedraw()\0languageChange()\0"
    "slotOnItem(QListWidgetItem*)\0"
    "slotCustomContextMenuRequested(QPoint)\0"
};

void ClusterPalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ClusterPalette *_t = static_cast<ClusterPalette *>(_o);
        switch (_id) {
        case 0: _t->singleChangeColor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateShownClusters((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 2: _t->groupClusters((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 3: _t->moveClustersToNoise((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 4: _t->moveClustersToArtefact((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 5: _t->clusterInformationModified(); break;
        case 6: _t->changeColor((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->moveClustersToNoise(); break;
        case 8: _t->moveClustersToArtefact(); break;
        case 9: _t->groupClusters(); break;
        case 10: _t->updateClusters(); break;
        case 11: _t->slotClickRedraw(); break;
        case 12: _t->languageChange(); break;
        case 13: _t->slotOnItem((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 14: _t->slotCustomContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ClusterPalette::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ClusterPalette::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ClusterPalette,
      qt_meta_data_ClusterPalette, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClusterPalette::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClusterPalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClusterPalette::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClusterPalette))
        return static_cast<void*>(const_cast< ClusterPalette*>(this));
    return QWidget::qt_metacast(_clname);
}

int ClusterPalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ClusterPalette::singleChangeColor(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClusterPalette::updateShownClusters(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClusterPalette::groupClusters(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ClusterPalette::moveClustersToNoise(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ClusterPalette::moveClustersToArtefact(const QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ClusterPalette::clusterInformationModified()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
