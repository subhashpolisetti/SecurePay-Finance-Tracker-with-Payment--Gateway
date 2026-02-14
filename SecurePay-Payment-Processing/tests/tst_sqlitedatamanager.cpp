#include <QtTest>
#include <QTemporaryDir>
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

    void cardNumberAndCvvAreNeverWrittenToDisk() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const std::string path = dir.filePath("securepay.db").toStdString();

        Customer alice("Alice Smith", "alice@example.com", "123 Main St");
        Merchant acme("Acme Store", "acme@example.com", "789 Market St");
        auto tx = TransactionFactory::createTransaction(
            alice, acme,
            PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "987"),
            75.0);
        std::string displayedDetails = tx->getPaymentMethod().getDetails();
        {
            SQLiteDataManager db(path);
            QVERIFY(db.initialize());
            QVERIFY(db.saveCustomer(alice));
            QVERIFY(db.saveMerchant(acme));
            QVERIFY(db.saveTransaction(*tx));

            auto loaded = db.loadTransactions(db.loadCustomers(), db.loadMerchants());
            QCOMPARE(loaded.size(), size_t(1));
            QCOMPARE(loaded[0]->getPaymentMethod().getDetails(), displayedDetails);
        }

        sqlite3* raw = nullptr;
        QCOMPARE(sqlite3_open(path.c_str(), &raw), SQLITE_OK);
        sqlite3_stmt* stmt = nullptr;
        QCOMPARE(sqlite3_prepare_v2(raw,
            "SELECT payment_detail1 || '|' || payment_detail2 || '|' || payment_detail3 || '|' || payment_detail4 "
            "FROM transactions;", -1, &stmt, nullptr), SQLITE_OK);
        QCOMPARE(sqlite3_step(stmt), SQLITE_ROW);
        std::string stored = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        sqlite3_close(raw);

        QCOMPARE(stored, std::string("1111|Alice Smith|12/30|"));
        QVERIFY(stored.find("4111111111111111") == std::string::npos);
        QVERIFY(stored.find("987") == std::string::npos);
    }

    void walletDetailsRoundTrip() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());
        Customer bob("Bob Johnson", "bob@example.com", "456 Oak Ave");
        Merchant acme("Acme Store", "acme@example.com", "789 Market St");
        QVERIFY(db.saveCustomer(bob));
        QVERIFY(db.saveMerchant(acme));
        auto tx = TransactionFactory::createTransaction(
            bob, acme, PaymentMethodFactory::createDigitalWallet("wallet-9", "bob@example.com"), 15.0);
        QVERIFY(db.saveTransaction(*tx));

        auto loaded = db.loadTransactions(db.loadCustomers(), db.loadMerchants());
        QCOMPARE(loaded[0]->getPaymentMethod().getDetails(), std::string("wallet-9 (bob@example.com)"));
    }

    void refundsAndFraudAlertsReloadWithOriginalIdentity() {
        SQLiteDataManager db(":memory:");
        QVERIFY(db.initialize());

        Customer alice("Alice Smith", "alice@example.com", "123 Main St");
        Merchant acme("Acme Store", "acme@example.com", "789 Market St");
        QVERIFY(db.saveCustomer(alice));
        QVERIFY(db.saveMerchant(acme));

        auto tx = TransactionFactory::createTransaction(
            alice, acme,
            PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123"),
            1500.0);
        tx->setState(std::make_unique<ApprovedState>());
        QVERIFY(tx->refund(100.0));
        auto refund = RefundFactory::createRefund(*tx, 100.0, "Late delivery");
        auto alert = FraudAlertFactory::createFraudAlert(*tx, FraudRiskLevel::MEDIUM, "Large amount");
        alert->setReviewed(true);

        QVERIFY(db.saveTransaction(*tx));
        QVERIFY(db.saveRefund(*refund));
        QVERIFY(db.saveFraudAlert(*alert));

        auto transactions = db.loadTransactions(db.loadCustomers(), db.loadMerchants());
        auto refunds = db.loadRefunds(transactions);
        auto alerts = db.loadFraudAlerts(transactions);

        QCOMPARE(refunds.size(), size_t(1));
        QCOMPARE(refunds[0]->getRefundId(), refund->getRefundId());
        QCOMPARE(refunds[0]->getTimestamp(), refund->getTimestamp());
        QCOMPARE(refunds[0]->getReason(), std::string("Late delivery"));
        QCOMPARE(&refunds[0]->getTransaction(), transactions[0].get());

        QCOMPARE(alerts.size(), size_t(1));
        QCOMPARE(alerts[0]->getAlertId(), alert->getAlertId());
        QCOMPARE(alerts[0]->getTimestamp(), alert->getTimestamp());
        QCOMPARE(alerts[0]->getRiskLevel(), FraudRiskLevel::MEDIUM);
        QVERIFY(alerts[0]->isReviewed());
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
