/****************************************************************************
** Meta object code from reading C++ file 'projectview.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "projectview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projectview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProjectView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProjectView[] = {
    "ProjectView\0\0showDockWidget(Tool*)\0"
    "linking()\0"
};

const QMetaObject ProjectView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_ProjectView,
      qt_meta_data_ProjectView, 0 }
};

const QMetaObject *ProjectView::metaObject() const
{
    return &staticMetaObject;
}

void *ProjectView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectView))
        return static_cast<void*>(const_cast< ProjectView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int ProjectView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showDockWidget((*reinterpret_cast< Tool*(*)>(_a[1]))); break;
        case 1: linking(); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ProjectView::showDockWidget(Tool * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
