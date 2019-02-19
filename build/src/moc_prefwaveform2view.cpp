/****************************************************************************
** Meta object code from reading C++ file 'prefwaveform2view.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/prefwaveform2view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'prefwaveform2view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PrefWaveform2View[] = {

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
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   18,   18,   18, 0x08,
      57,   18,   18,   18, 0x08,
      76,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PrefWaveform2View[] = {
    "PrefWaveform2View\0\0positionsChanged()\0"
    "saveChannelOrder()\0loadChannelOrder()\0"
    "updateChannelPositions()\0"
};

void PrefWaveform2View::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrefWaveform2View *_t = static_cast<PrefWaveform2View *>(_o);
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

const QMetaObjectExtraData PrefWaveform2View::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrefWaveform2View::staticMetaObject = {
    { &PrefWaveform2ViewLayout::staticMetaObject, qt_meta_stringdata_PrefWaveform2View,
      qt_meta_data_PrefWaveform2View, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrefWaveform2View::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrefWaveform2View::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrefWaveform2View::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrefWaveform2View))
        return static_cast<void*>(const_cast< PrefWaveform2View*>(this));
    return PrefWaveform2ViewLayout::qt_metacast(_clname);
}

int PrefWaveform2View::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PrefWaveform2ViewLayout::qt_metacall(_c, _id, _a);
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
void PrefWaveform2View::positionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
