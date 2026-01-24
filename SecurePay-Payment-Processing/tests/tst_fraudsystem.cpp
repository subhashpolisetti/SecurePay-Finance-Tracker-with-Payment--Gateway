#include <QtTest>
#include "fraudsystem.h"
#include "transaction.h"

namespace {

std::unique_ptr<Transaction> makeTransaction(const std::string& billingAddress, double amount,
                                             bool useWallet = false) {
    auto method = useWallet
        ? PaymentMethodFactory::createDigitalWallet("wallet-1", "alice@example.com")
        : PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123");
    return TransactionFactory::createTransaction(
        Customer("Alice Smith", "alice@example.com", billingAddress),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        std::move(method), amount);
}

FraudRiskLevel evaluate(const Transaction& tx) {
    return FraudSystem::getInstance().evaluateTransaction(tx);
}

} // namespace

class TestFraudSystem : public QObject {
    Q_OBJECT

private slots:
    void ordinaryCardPaymentIsLowRisk() {
        auto tx = makeTransaction("123 Main St, San Francisco, CA", 50.0);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::LOW);
    }

    void largeAmountIsMediumRisk() {
        auto tx = makeTransaction("123 Main St", FraudRules::kLargeAmountThreshold + 0.01);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::MEDIUM);
    }

    void amountAtThresholdIsNotSuspicious() {
        auto tx = makeTransaction("123 Main St", FraudRules::kLargeAmountThreshold);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::LOW);
    }

    void suspiciousAddressIsMediumRiskRegardlessOfCase() {
        auto tx = makeTransaction("UNKNOWN location", 50.0);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::MEDIUM);
    }

    void walletPaymentIsMediumRisk() {
        auto tx = makeTransaction("123 Main St", 50.0, true);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::MEDIUM);
    }

    void twoOrMoreFactorsAreHighRisk() {
        auto tx = makeTransaction("test address", 2500.0, true);
        QCOMPARE(evaluate(*tx), FraudRiskLevel::HIGH);
    }

    void riskLevelsHaveReadableNames() {
        QCOMPARE(FraudSystem::riskLevelToString(FraudRiskLevel::LOW), std::string("Low"));
        QCOMPARE(FraudSystem::riskLevelToString(FraudRiskLevel::MEDIUM), std::string("Medium"));
        QCOMPARE(FraudSystem::riskLevelToString(FraudRiskLevel::HIGH), std::string("High"));
    }
};

QTEST_APPLESS_MAIN(TestFraudSystem)
#include "tst_fraudsystem.moc"
