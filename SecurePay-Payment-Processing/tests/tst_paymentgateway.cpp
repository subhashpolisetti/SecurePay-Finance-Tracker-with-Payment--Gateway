#include <QtTest>
#include "paymentgateway.h"

namespace {

// Each call uses a fresh customer so the fraud velocity rule does not leak between tests
std::unique_ptr<Transaction> makeTransaction(double amount, const std::string& cardNumber = "4111111111111111",
                                             const std::string& billingAddress = "123 Main St") {
    static int nextCustomer = 0;
    return TransactionFactory::createTransaction(
        Customer("Gateway Customer " + std::to_string(++nextCustomer), "gw@example.com", billingAddress),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        PaymentMethodFactory::createCreditCard(cardNumber, "Alice Smith", "12/30", "123"),
        amount);
}

} // namespace

class TestPaymentGateway : public QObject {
    Q_OBJECT

private slots:
    void lowRiskPaymentIsApprovedWithoutAlert() {
        PaymentGateway gateway;
        gateway.processTransaction(makeTransaction(50.0));

        QCOMPARE(gateway.getTransactions().size(), size_t(1));
        QCOMPARE(gateway.getTransactions()[0]->getStatus(), TransactionStatus::APPROVED);
        QVERIFY(gateway.getFraudAlerts().empty());
    }

    void invalidCardIsDeclined() {
        PaymentGateway gateway;
        gateway.processTransaction(makeTransaction(50.0, "4111111111111112"));
        QCOMPARE(gateway.getTransactions()[0]->getStatus(), TransactionStatus::DECLINED);
    }

    void mediumRiskPaymentIsApprovedAndAlerted() {
        PaymentGateway gateway;
        auto tx = makeTransaction(1500.0);
        std::string id = tx->getTransactionId();
        gateway.processTransaction(std::move(tx));

        QCOMPARE(gateway.getTransactions()[0]->getStatus(), TransactionStatus::APPROVED);
        QCOMPARE(gateway.getFraudAlerts().size(), size_t(1));
        const FraudAlert& alert = *gateway.getFraudAlerts()[0];
        QCOMPARE(alert.getRiskLevel(), FraudRiskLevel::MEDIUM);
        QCOMPARE(alert.getTransaction().getTransactionId(), id);
        QVERIFY(!alert.isReviewed());
    }

    void highRiskPaymentIsFlaggedAndAlerted() {
        PaymentGateway gateway;
        gateway.processTransaction(makeTransaction(1500.0, "4111111111111111", "unknown address"));

        QCOMPARE(gateway.getTransactions()[0]->getStatus(), TransactionStatus::FLAGGED_FOR_REVIEW);
        QCOMPARE(gateway.getFraudAlerts().size(), size_t(1));
        QCOMPARE(gateway.getFraudAlerts()[0]->getRiskLevel(), FraudRiskLevel::HIGH);
    }
};

QTEST_APPLESS_MAIN(TestPaymentGateway)
#include "tst_paymentgateway.moc"
