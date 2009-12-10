/****************************************************************************
** Meta object code from reading C++ file 'projectpropertiesdialog.h'
**
** Created: Thu Dec 10 11:36:43 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../projectpropertiesdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projectpropertiesdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProjectPropertiesDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   25,   24,   24, 0x08,
      63,   25,   24,   24, 0x08,
      93,   25,   24,   24, 0x08,
     121,   24,   24,   24, 0x08,
     158,   24,   24,   24, 0x08,
     195,   24,   24,   24, 0x08,
     242,  231,   24,   24, 0x08,
     286,  280,   24,   24, 0x08,
     315,  280,   24,   24, 0x08,
     345,  280,   24,   24, 0x08,
     376,  231,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProjectPropertiesDialog[] = {
    "ProjectPropertiesDialog\0\0position\0"
    "on_blueDial_sliderMoved(int)\0"
    "on_greenDial_sliderMoved(int)\0"
    "on_redDial_sliderMoved(int)\0"
    "on_ImportPalettePushButton_clicked()\0"
    "on_exportPalettePushButton_clicked()\0"
    "on_resetPalettePushButton_clicked()\0"
    "row,column\0on_tableWidget_cellActivated(int,int)\0"
    "value\0on_redDial_valueChanged(int)\0"
    "on_blueDial_valueChanged(int)\0"
    "on_greenDial_valueChanged(int)\0"
    "on_tableWidget_cellClicked(int,int)\0"
};

const QMetaObject ProjectPropertiesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ProjectPropertiesDialog,
      qt_meta_data_ProjectPropertiesDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProjectPropertiesDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProjectPropertiesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProjectPropertiesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectPropertiesDialog))
        return static_cast<void*>(const_cast< ProjectPropertiesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ProjectPropertiesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_blueDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_greenDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: on_redDial_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_ImportPalettePushButton_clicked(); break;
        case 4: on_exportPalettePushButton_clicked(); break;
        case 5: on_resetPalettePushButton_clicked(); break;
        case 6: on_tableWidget_cellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: on_redDial_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: on_blueDial_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: on_greenDial_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: on_tableWidget_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
