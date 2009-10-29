/****************************************************************************
** Meta object code from reading C++ file 'importwizard.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "importwizard.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'importwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImportWizard[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      59,   13,   13,   13, 0x0a,
      73,   13,   13,   13, 0x0a,
      81,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ImportWizard[] = {
    "ImportWizard\0\0,,\0"
    "createToolButton(QString,QString,QString)\0"
    "browseFiles()\0clear()\0check(int)\0"
};

const QMetaObject ImportWizard::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_ImportWizard,
      qt_meta_data_ImportWizard, 0 }
};

const QMetaObject *ImportWizard::metaObject() const
{
    return &staticMetaObject;
}

void *ImportWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImportWizard))
        return static_cast<void*>(const_cast< ImportWizard*>(this));
    return QWizard::qt_metacast(_clname);
}

int ImportWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: createToolButton((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: browseFiles(); break;
        case 2: clear(); break;
        case 3: check((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ImportWizard::createToolButton(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
