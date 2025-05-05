/****************************************************************************
** Meta object code from reading C++ file 'customerview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gui/customerview.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customerview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12CustomerViewE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN12CustomerViewE = QtMocHelpers::stringData(
    "CustomerView",
    "transactionUpdated",
    "",
    "Transaction",
    "transaction",
    "onCustomerSelected",
    "index",
    "onAddCustomerClicked",
    "onPaymentMethodSelected",
    "onSavedCardSelected",
    "onManageCardsClicked",
    "onSubmitClicked",
    "onExportCustomerReportClicked",
    "onDepositClicked",
    "onCheckBalanceClicked",
    "onLoginClicked",
    "onLogoutClicked",
    "onOpenCheckoutClicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN12CustomerViewE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   92,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   95,    2, 0x08,    3 /* Private */,
       7,    0,   98,    2, 0x08,    5 /* Private */,
       8,    1,   99,    2, 0x08,    6 /* Private */,
       9,    1,  102,    2, 0x08,    8 /* Private */,
      10,    0,  105,    2, 0x08,   10 /* Private */,
      11,    0,  106,    2, 0x08,   11 /* Private */,
      12,    0,  107,    2, 0x08,   12 /* Private */,
      13,    0,  108,    2, 0x08,   13 /* Private */,
      14,    0,  109,    2, 0x08,   14 /* Private */,
      15,    0,  110,    2, 0x08,   15 /* Private */,
      16,    0,  111,    2, 0x08,   16 /* Private */,
      17,    0,  112,    2, 0x08,   17 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject CustomerView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN12CustomerViewE.offsetsAndSizes,
    qt_meta_data_ZN12CustomerViewE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN12CustomerViewE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CustomerView, std::true_type>,
        // method 'transactionUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Transaction &, std::false_type>,
        // method 'onCustomerSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onAddCustomerClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPaymentMethodSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSavedCardSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onManageCardsClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSubmitClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportCustomerReportClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDepositClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCheckBalanceClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLoginClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLogoutClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOpenCheckoutClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CustomerView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CustomerView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->transactionUpdated((*reinterpret_cast< std::add_pointer_t<Transaction>>(_a[1]))); break;
        case 1: _t->onCustomerSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onAddCustomerClicked(); break;
        case 3: _t->onPaymentMethodSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onSavedCardSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onManageCardsClicked(); break;
        case 6: _t->onSubmitClicked(); break;
        case 7: _t->onExportCustomerReportClicked(); break;
        case 8: _t->onDepositClicked(); break;
        case 9: _t->onCheckBalanceClicked(); break;
        case 10: _t->onLoginClicked(); break;
        case 11: _t->onLogoutClicked(); break;
        case 12: _t->onOpenCheckoutClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (CustomerView::*)(const Transaction & );
            if (_q_method_type _q_method = &CustomerView::transactionUpdated; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *CustomerView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomerView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN12CustomerViewE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CustomerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void CustomerView::transactionUpdated(const Transaction & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
