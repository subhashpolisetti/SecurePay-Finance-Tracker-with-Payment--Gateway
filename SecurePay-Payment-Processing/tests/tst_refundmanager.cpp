#include <QtTest>
#include "refundmanager.h"

namespace {

// RefundManager is a singleton whose refunds reference their transaction, so test
// transactions must outlive every test case, as they do inside PaymentGateway.
Transaction* makeApprovedTransaction(double amount) {
    static std::vector<std::unique_ptr<Transaction>> keepAlive;
    auto tx = TransactionFactory::createTransaction(
        Customer("Refund Customer", "refund@example.com", "123 Main St"),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        PaymentMethodFactory::createCreditCard("4111111111111111", "Refund Customer", "12/30", "123"),
        amount);
    tx->setState(std::make_unique<ApprovedState>());
    keepAlive.push_back(std::move(tx));
    return keepAlive.back().get();
}

} // namespace

class TestRefundManager : public QObject {
    Q_OBJECT

private slots:
    void releasedRefundIsTheOneTheCommandCreated() {
        auto tx = makeApprovedTransaction(80.0);
        PartialRefundCommand command(*tx, 30.0, "Damaged item");
        QVERIFY(command.execute());

        std::string createdId = command.getRefund()->getRefundId();
        std::unique_ptr<Refund> released = command.releaseRefund();
        QVERIFY(released);
        QCOMPARE(released->getRefundId(), createdId);
        QVERIFY(command.getRefund() == nullptr);
    }

    void failedCommandHasNoRefund() {
        auto tx = makeApprovedTransaction(80.0);
        PartialRefundCommand command(*tx, 500.0, "More than paid");
        QVERIFY(!command.execute());
        QVERIFY(command.releaseRefund() == nullptr);
    }

    void partialThenFullRefundIsRecordedPerTransaction() {
        auto tx = makeApprovedTransaction(100.0);
        RefundManager& manager = RefundManager::getInstance();

        QVERIFY(manager.processPartialRefund(*tx, 40.0, "Partial return"));
        QCOMPARE(tx->getStatus(), TransactionStatus::PARTIALLY_REFUNDED);
        QVERIFY(manager.processFullRefund(*tx, "Rest of order"));
        QCOMPARE(tx->getStatus(), TransactionStatus::REFUNDED);

        auto refunds = manager.getRefundsForTransaction(tx->getTransactionId());
        QCOMPARE(refunds.size(), size_t(2));
        QCOMPARE(refunds[0]->getAmount(), 40.0);
        QCOMPARE(refunds[1]->getAmount(), 60.0);
        QCOMPARE(refunds[1]->getReason(), std::string("Rest of order"));
    }

    void refundRejectedOnceFullyRefunded() {
        auto tx = makeApprovedTransaction(25.0);
        RefundManager& manager = RefundManager::getInstance();
        QVERIFY(manager.processFullRefund(*tx, "Cancelled"));
        QVERIFY(!manager.processPartialRefund(*tx, 1.0, "Again"));
        QCOMPARE(manager.getRefundsForTransaction(tx->getTransactionId()).size(), size_t(1));
    }
};

QTEST_APPLESS_MAIN(TestRefundManager)
#include "tst_refundmanager.moc"
