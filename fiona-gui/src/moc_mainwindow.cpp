/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   11,   11,   11, 0x0a,
      47,   11,   11,   11, 0x0a,
      61,   11,   11,   11, 0x0a,
      76,   11,   11,   11, 0x0a,
      95,   11,   11,   11, 0x0a,
     108,   11,   11,   11, 0x0a,
     122,   11,   11,   11, 0x0a,
     135,   11,   11,   11, 0x0a,
     160,  157,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0browsedFiles(QString)\0"
    "newProject()\0openProject()\0closeProject()\0"
    "closeAllProjects()\0importTool()\0"
    "browseFiles()\0createItem()\0"
    "showDockWidget(Tool*)\0,,\0"
    "createToolButton(QString,QString,QString)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: browsedFiles((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: newProject(); break;
        case 2: openProject(); break;
        case 3: closeProject(); break;
        case 4: closeAllProjects(); break;
        case 5: importTool(); break;
        case 6: browseFiles(); break;
        case 7: createItem(); break;
        case 8: showDockWidget((*reinterpret_cast< Tool*(*)>(_a[1]))); break;
        case 9: createToolButton((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::browsedFiles(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
