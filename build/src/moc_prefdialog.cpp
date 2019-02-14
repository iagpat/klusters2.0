/****************************************************************************
** Meta object code from reading C++ file 'prefdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/prefdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'prefdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PrefDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   11,   11,   11, 0x0a,
      44,   11,   11,   11, 0x0a,
      56,   11,   11,   11, 0x0a,
      70,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrefDialog[] = {
    "PrefDialog\0\0settingsChanged()\0"
    "slotDefault()\0slotApply()\0enableApply()\0"
    "slotHelp()\0"
};

void PrefDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrefDialog *_t = static_cast<PrefDialog *>(_o);
        switch (_id) {
        case 0: _t->settingsChanged(); break;
        case 1: _t->slotDefault(); break;
        case 2: _t->slotApply(); break;
        case 3: _t->enableApply(); break;
        case 4: _t->slotHelp(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PrefDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrefDialog::staticMetaObject = {
    { &QPageDialog::staticMetaObject, qt_meta_stringdata_PrefDialog,
      qt_meta_data_PrefDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrefDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrefDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrefDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrefDialog))
        return static_cast<void*>(const_cast< PrefDialog*>(this));
    return QPageDialog::qt_metacast(_clname);
}

int PrefDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPageDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void PrefDialog::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
