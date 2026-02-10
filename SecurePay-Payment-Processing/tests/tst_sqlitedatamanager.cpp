#include <QtTest>
#include "sqlitedatamanager.h"

class TestSQLiteDataManager : public QObject {
    Q_OBJECT

private slots:
    void customerNamesWithQuotesAreStoredLiterally() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());

        QVERIFY(db.saveCustomer(Customer("Conan O'Brien", "conan@example.com", "1 Late Night Ave")));

        auto customers = db.loadCustomers();
        QCOMPARE(customers.size(), size_t(1));
        QCOMPARE(customers[0].getName(), std::string("Conan O'Brien"));
    }

    void injectionPayloadCannotAlterTheDatabase() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());
        QVERIFY(db.saveMerchant(Merchant("Acme Store", "acme@example.com", "789 Market St")));

        const std::string payload = "x', '', ''); DROP TABLE merchants; --";
        QVERIFY(db.saveMerchant(Merchant(payload, "evil@example.com", "nowhere")));

        auto merchants = db.loadMerchants();
        QCOMPARE(merchants.size(), size_t(2));
        bool storedAsData = false;
        for (const auto& merchant : merchants) {
            storedAsData = storedAsData || merchant.getName() == payload;
        }
        QVERIFY(storedAsData);
    }

    void transactionsReloadWithOriginalIdStatusAndRefunds() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());

        Customer alice("Alice Smith", "alice@example.com", "123 Main St");
        Merchant acme("Acme Store", "acme@example.com", "789 Market St");
        QVERIFY(db.saveCustomer(alice));
        QVERIFY(db.saveMerchant(acme));

        auto tx = TransactionFactory::createTransaction(
            alice, acme,
            PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123"),
            120.0);
        tx->setState(std::make_unique<ApprovedState>());
        QVERIFY(tx->refund(20.0));
        QVERIFY(db.saveTransaction(*tx));

        auto loaded = db.loadTransactions(db.loadCustomers(), db.loadMerchants());
        QCOMPARE(loaded.size(), size_t(1));
        QCOMPARE(loaded[0]->getTransactionId(), tx->getTransactionId());
        QCOMPARE(loaded[0]->getStatus(), TransactionStatus::PARTIALLY_REFUNDED);
        QCOMPARE(loaded[0]->getAmount(), 120.0);
        QCOMPARE(loaded[0]->getRefundedAmount(), 20.0);
        QCOMPARE(loaded[0]->getTimestamp(), tx->getTimestamp());
        QCOMPARE(loaded[0]->getPaymentMethod().getType(), std::string("Credit Card"));
    }

    void refundReasonWithQuotesIsSaved() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());

        auto tx = TransactionFactory::createTransaction(
            Customer("Alice Smith", "alice@example.com", "123 Main St"),
            Merchant("Acme Store", "acme@example.com", "789 Market St"),
            PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123"),
            40.0);
        auto refund = RefundFactory::createRefund(*tx, 10.0, "Customer's item didn't arrive");
        QVERIFY(db.saveTransaction(*tx));
        QVERIFY(db.saveRefund(*refund));
    }
};

QTEST_APPLESS_MAIN(TestSQLiteDataManager)
#include "tst_sqlitedatamanager.moc"
