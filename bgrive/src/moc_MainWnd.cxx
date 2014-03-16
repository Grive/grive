/****************************************************************************
** Meta object code from reading C++ file 'MainWnd.hh'
**
** Created: Sun Mar 16 21:47:10 2014
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWnd.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWnd.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gr__MainWnd[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x0a,
      44,   12,   12,   12, 0x0a,
      63,   12,   12,   12, 0x0a,
      89,   83,   12,   12, 0x0a,
     118,  111,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gr__MainWnd[] = {
    "gr::MainWnd\0\0index\0OnClickItem(QModelIndex)\0"
    "OnClickDetailTab()\0OnClickPreviewTab()\0"
    "event\0OnClose(QCloseEvent*)\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
};

const QMetaObject gr::MainWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_gr__MainWnd,
      qt_meta_data_gr__MainWnd, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gr::MainWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gr::MainWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gr::MainWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gr__MainWnd))
        return static_cast<void*>(const_cast< MainWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int gr::MainWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnClickItem((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: OnClickDetailTab(); break;
        case 2: OnClickPreviewTab(); break;
        case 3: OnClose((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 4: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
