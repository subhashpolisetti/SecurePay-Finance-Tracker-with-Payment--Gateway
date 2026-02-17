#include <QtTest>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "gui/mainwindow.h"

namespace {

template <typename T>
T* widget(MainWindow& window, const char* name) {
    T* found = window.findChild<T*>(name);
    if (!found) {
        qFatal("Widget %s not found", name);
    }
    return found;
}

void payByCard(MainWindow& window, int customerIndex, int merchantIndex, const QString& amount) {
    widget<QComboBox>(window, "customerComboBox")->setCurrentIndex(customerIndex);
    widget<QComboBox>(window, "payeeMerchantComboBox")->setCurrentIndex(merchantIndex);
    widget<QComboBox>(window, "paymentMethodComboBox")->setCurrentText("Credit Card");
    widget<QLineEdit>(window, "amountEdit")->setText(amount);
    widget<QLineEdit>(window, "cardNumberEdit")->setText("4111111111111111");
    widget<QLineEdit>(window, "cardholderNameEdit")->setText("Alice Smith");
    widget<QLineEdit>(window, "expiryDateEdit")->setText("12/30");
    widget<QLineEdit>(window, "cvvEdit")->setText("123");
    widget<QPushButton>(window, "submitButton")->click();
}

} // namespace

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void paymentGoesToSelectedMerchant() {
        MainWindow window;
        auto* payee = widget<QComboBox>(window, "payeeMerchantComboBox");
        QCOMPARE(payee->count(), 2);
        QCOMPARE(payee->itemText(1), QString("XYZ Electronics"));

        payByCard(window, 0, 1, "25.00");

        auto* table = widget<QTableWidget>(window, "merchantTransactionTable");
        QCOMPARE(table->rowCount(), 1);
        QCOMPARE(table->item(0, 2)->text(), QString("XYZ Electronics"));
        QCOMPARE(table->item(0, 5)->text(), QString("Approved"));
        QCOMPARE(widget<QLabel>(window, "resultLabel")->text(), QString("Transaction Approved"));
    }
};

QTEST_MAIN(TestMainWindow)
#include "tst_mainwindow.moc"
