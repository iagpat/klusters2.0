/****************************************************************************
** Meta object code from reading C++ file 'prefwaveformview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/prefwaveformview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'prefwaveformview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PrefWaveformView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   17,   17,   17, 0x08,
      56,   17,   17,   17, 0x08,
      75,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PrefWaveformView[] = {
    "PrefWaveformView\0\0positionsChanged()\0"
    "saveChannelOrder()\0loadChannelOrder()\0"
    "updateChannelPositions()\0"
};

void PrefWaveformView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrefWaveformView *_t = static_cast<PrefWaveformView *>(_o);
        switch (_id) {
        case 0: _t->positionsChanged(); break;
        case 1: _t->saveChannelOrder(); break;
        case 2: _t->loadChannelOrder(); break;
        case 3: _t->updateChannelPositions(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PrefWaveformView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrefWaveformView::staticMetaObject = {
    { &PrefWaveformViewLayout::staticMetaObject, qt_meta_stringdata_PrefWaveformView,
      qt_meta_data_PrefWaveformView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrefWaveformView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrefWaveformView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrefWaveformView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrefWaveformView))
        return static_cast<void*>(const_cast< PrefWaveformView*>(this));
    return PrefWaveformViewLayout::qt_metacast(_clname);
}

int PrefWaveformView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PrefWaveformViewLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PrefWaveformView::positionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
