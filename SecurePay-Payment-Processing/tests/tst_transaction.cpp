#include <QtTest>
#include "transaction.h"

namespace {

std::unique_ptr<Transaction> makeTransaction(double amount) {
    return TransactionFactory::createTransaction(
        Customer("State Customer", "state@example.com", "123 Main St"),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        PaymentMethodFactory::createCreditCard("4111111111111111", "State Customer", "12/30", "123"),
        amount);
}

} // namespace

class TestTransaction : public QObject {
    Q_OBJECT

private slots:
    void newTransactionIsPendingWithUniqueId() {
        auto a = makeTransaction(10.0);
        auto b = makeTransaction(10.0);
        QCOMPARE(a->getStatus(), TransactionStatus::PENDING);
        QVERIFY(a->getTransactionId().rfind("TX-", 0) == 0);
        QVERIFY(a->getTransactionId() != b->getTransactionId());
    }

    void pendingCannotBeRefunded() {
        auto tx = makeTransaction(10.0);
        QVERIFY(!tx->refund(5.0));
        QCOMPARE(tx->getRefundedAmount(), 0.0);
    }

    void processingPendingApprovesIt() {
        auto tx = makeTransaction(10.0);
        QVERIFY(tx->process());
        QCOMPARE(tx->getStatus(), TransactionStatus::APPROVED);
        QVERIFY(!tx->process());
    }

    void partialRefundsThenFullRefund() {
        auto tx = makeTransaction(100.0);
        tx->setState(std::make_unique<ApprovedState>());

        QVERIFY(tx->refund(30.0));
        QCOMPARE(tx->getStatus(), TransactionStatus::PARTIALLY_REFUNDED);
        QCOMPARE(tx->getRemainingAmount(), 70.0);

        QVERIFY(tx->refund(70.0));
        QCOMPARE(tx->getStatus(), TransactionStatus::REFUNDED);
        QVERIFY(!tx->refund(1.0));
    }

    void refundRejectsNonPositiveAndExcessAmounts() {
        auto tx = makeTransaction(50.0);
        tx->setState(std::make_unique<ApprovedState>());
        QVERIFY(!tx->refund(0.0));
        QVERIFY(!tx->refund(-5.0));
        QVERIFY(!tx->refund(50.01));
        QCOMPARE(tx->getStatus(), TransactionStatus::APPROVED);
    }

    void centAmountsRefundExactlyDespiteFloatingPoint() {
        auto tx = makeTransaction(0.30);
        tx->setState(std::make_unique<ApprovedState>());
        QVERIFY(tx->refund(0.10));
        QVERIFY(tx->refund(0.10));
        QVERIFY(tx->refund(0.10));
        QCOMPARE(tx->getStatus(), TransactionStatus::REFUNDED);
    }

    void declinedAndFlaggedCannotBeRefunded() {
        auto declined = makeTransaction(10.0);
        declined->setState(std::make_unique<DeclinedState>());
        QVERIFY(!declined->refund(5.0));

        auto flagged = makeTransaction(10.0);
        flagged->setState(std::make_unique<FlaggedState>());
        QVERIFY(!flagged->refund(5.0));
        QVERIFY(!flagged->process());
    }

    void statusNamesAreReadable() {
        QCOMPARE(Transaction::statusToString(TransactionStatus::FLAGGED_FOR_REVIEW), std::string("Flagged for Review"));
        QCOMPARE(Transaction::statusToString(TransactionStatus::PARTIALLY_REFUNDED), std::string("Partially Refunded"));
    }
};

QTEST_APPLESS_MAIN(TestTransaction)
#include "tst_transaction.moc"
