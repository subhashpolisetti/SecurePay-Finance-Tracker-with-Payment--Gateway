#include <QtTest>
#include "paymentmethod.h"

class TestPaymentMethod : public QObject {
    Q_OBJECT

private slots:
    void creditCardReportsTypeAndMaskedDetails() {
        auto card = PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123");
        QCOMPARE(card->getType(), std::string("Credit Card"));
        QCOMPARE(card->getDetails(), std::string("XXXX-XXXX-XXXX-1111 (Alice Smith)"));
    }

    void debitCardReportsTypeAndMaskedDetails() {
        auto card = PaymentMethodFactory::createDebitCard("5500000000000004", "Bob Johnson", "01/29", "456");
        QCOMPARE(card->getType(), std::string("Debit Card"));
        QCOMPARE(card->getDetails(), std::string("XXXX-XXXX-XXXX-0004 (Bob Johnson)"));
    }

    void maskedDetailsNeverContainFullCardNumber() {
        auto card = PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123");
        QVERIFY(card->getDetails().find("4111111111111111") == std::string::npos);
        QVERIFY(card->getDetails().find("123") == std::string::npos);
    }

    void shortOrEmptyCardNumberIsFullyMasked() {
        auto empty = PaymentMethodFactory::createCreditCard("", "Alice Smith", "12/30", "123");
        QCOMPARE(empty->getDetails(), std::string("XXXX-XXXX-XXXX-XXXX (Alice Smith)"));

        auto shortNumber = PaymentMethodFactory::createDebitCard("123", "Bob Johnson", "12/30", "123");
        QCOMPARE(shortNumber->getDetails(), std::string("XXXX-XXXX-XXXX-XXXX (Bob Johnson)"));
    }

    void digitalWalletReportsIdAndEmail() {
        auto wallet = PaymentMethodFactory::createDigitalWallet("wallet-42", "alice@example.com");
        QCOMPARE(wallet->getType(), std::string("Digital Wallet"));
        QCOMPARE(wallet->getDetails(), std::string("wallet-42 (alice@example.com)"));
    }

    void processRespectsPerMethodLimits() {
        auto credit = PaymentMethodFactory::createCreditCard("4111111111111111", "A", "12/30", "123");
        auto debit = PaymentMethodFactory::createDebitCard("5500000000000004", "B", "12/30", "123");
        auto wallet = PaymentMethodFactory::createDigitalWallet("w", "w@example.com");

        QVERIFY(credit->process(9999.99));
        QVERIFY(!credit->process(10000.0));
        QVERIFY(debit->process(4999.99));
        QVERIFY(!debit->process(5000.0));
        QVERIFY(wallet->process(1999.99));
        QVERIFY(!wallet->process(2000.0));
    }

    void cloneProducesEquivalentPaymentMethod() {
        auto card = PaymentMethodFactory::createCreditCard("4111111111111111", "Alice Smith", "12/30", "123");
        std::unique_ptr<PaymentMethod> copy(card->clone());
        QCOMPARE(copy->getType(), card->getType());
        QCOMPARE(copy->getDetails(), card->getDetails());
    }
};

QTEST_APPLESS_MAIN(TestPaymentMethod)
#include "tst_paymentmethod.moc"
