#include <QtTest>
#include "bank.h"
#include "transaction.h"

namespace {

std::unique_ptr<Transaction> makeCardTransaction(const std::string& cardNumber, double amount,
                                                 const std::string& expiry = "12/30") {
    return TransactionFactory::createTransaction(
        Customer("Alice Smith", "alice@example.com", "123 Main St"),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        PaymentMethodFactory::createCreditCard(cardNumber, "Alice Smith", expiry, "123"),
        amount);
}

} // namespace

class TestBank : public QObject {
    Q_OBJECT

private slots:
    void approvesValidCardWithLowRisk() {
        auto tx = makeCardTransaction("4111111111111111", 50.0);
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::LOW),
                 AuthorizationResult::APPROVED);
    }

    void declinesCardThatFailsLuhnCheck() {
        auto tx = makeCardTransaction("4111111111111112", 50.0);
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::LOW),
                 AuthorizationResult::DECLINED);
    }

    void declinesExpiredCard() {
        auto tx = makeCardTransaction("4111111111111111", 50.0, "01/20");
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::LOW),
                 AuthorizationResult::DECLINED);
    }

    void declinesAmountsAboveAvailableFunds() {
        auto tx = makeCardTransaction("4111111111111111", 5000.0);
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::LOW),
                 AuthorizationResult::DECLINED);
    }

    void sendsHighRiskTransactionsToReview() {
        auto tx = makeCardTransaction("4111111111111111", 50.0);
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::HIGH),
                 AuthorizationResult::REVIEW_REQUIRED);
    }

    void approvesMediumRiskTransactions() {
        auto tx = makeCardTransaction("4111111111111111", 50.0);
        QCOMPARE(Bank::getInstance().authorizeTransaction(*tx, FraudRiskLevel::MEDIUM),
                 AuthorizationResult::APPROVED);
    }
};

QTEST_APPLESS_MAIN(TestBank)
#include "tst_bank.moc"
