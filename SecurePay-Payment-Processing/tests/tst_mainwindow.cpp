#include <QtTest>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
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

        widget<QComboBox>(window, "merchantComboBox")->setCurrentIndex(1);
        auto* table = widget<QTableWidget>(window, "merchantTransactionTable");
        QCOMPARE(table->rowCount(), 1);
        QCOMPARE(table->item(0, 2)->text(), QString("XYZ Electronics"));
        QCOMPARE(table->item(0, 5)->text(), QString("Approved"));
        QCOMPARE(widget<QLabel>(window, "resultLabel")->text(), QString("Transaction Approved"));
    }

    void historyTablesShowOnlySelectedCustomerAndMerchant() {
        MainWindow window;
        payByCard(window, 0, 0, "10.00");   // Alice -> Acme Store
        payByCard(window, 1, 1, "20.00");   // Bob -> XYZ Electronics
        payByCard(window, 1, 0, "30.00");   // Bob -> Acme Store

        auto* customers = widget<QComboBox>(window, "customerComboBox");
        auto* customerTable = widget<QTableWidget>(window, "customerTransactionTable");
        customers->setCurrentIndex(0);
        QCOMPARE(customerTable->rowCount(), 1);
        QCOMPARE(customerTable->item(0, 1)->text(), QString("Alice Smith"));
        customers->setCurrentIndex(1);
        QCOMPARE(customerTable->rowCount(), 2);

        auto* merchants = widget<QComboBox>(window, "merchantComboBox");
        auto* merchantTable = widget<QTableWidget>(window, "merchantTransactionTable");
        merchants->setCurrentIndex(0);
        QCOMPARE(merchantTable->rowCount(), 2);
        merchants->setCurrentIndex(1);
        QCOMPARE(merchantTable->rowCount(), 1);
        QCOMPARE(merchantTable->item(0, 1)->text(), QString("Bob Johnson"));
    }

    void fraudAlertsTabListsMerchantAlerts() {
        MainWindow window;
        payByCard(window, 0, 0, "1500.00");   // above the large-amount threshold -> medium risk

        auto* merchants = widget<QComboBox>(window, "merchantComboBox");
        auto* alerts = widget<QTableWidget>(window, "fraudAlertTable");
        merchants->setCurrentIndex(0);
        QCOMPARE(alerts->rowCount(), 1);
        QCOMPARE(alerts->item(0, 2)->text(), QString("Medium"));
        merchants->setCurrentIndex(1);
        QCOMPARE(alerts->rowCount(), 0);
    }

    // Keep last: RefundManager is a process-wide singleton holding references to this window's transactions
    void refundsTabListsMerchantRefunds() {
        MainWindow window;
        payByCard(window, 0, 1, "80.00");

        widget<QComboBox>(window, "merchantComboBox")->setCurrentIndex(1);
        auto* transactions = widget<QTableWidget>(window, "merchantTransactionTable");
        QCOMPARE(transactions->rowCount(), 1);
        transactions->setCurrentCell(0, 0);

        // The refund dialog and its confirmation box are modal; answer them from inside their event loops
        QTimer::singleShot(0, [] {
            auto* dialog = QApplication::activeModalWidget();
            QVERIFY(dialog);
            dialog->findChild<QTextEdit*>()->setPlainText("Damaged on arrival");
            QTimer::singleShot(0, [] {
                auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
                QVERIFY(box);
                box->accept();
            });
            for (auto* button : dialog->findChildren<QPushButton*>()) {
                if (button->text() == "Process Refund") {
                    button->click();
                }
            }
        });
        widget<QPushButton>(window, "processRefundButton")->click();

        auto* refunds = widget<QTableWidget>(window, "refundTable");
        QCOMPARE(refunds->rowCount(), 1);
        QCOMPARE(refunds->item(0, 2)->text(), QString("$80.00"));
        QCOMPARE(refunds->item(0, 3)->text(), QString("Damaged on arrival"));
        QCOMPARE(transactions->item(0, 5)->text(), QString("Refunded"));
    }
};

QTEST_MAIN(TestMainWindow)
#include "tst_mainwindow.moc"
