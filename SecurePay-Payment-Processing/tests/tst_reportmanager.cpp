#include <QtTest>
#include "reportmanager.h"
#include "paymentgateway.h"
#include "refundmanager.h"

namespace {

std::unique_ptr<Transaction> makeTransaction(const std::string& customerName, double amount) {
    return TransactionFactory::createTransaction(
        Customer(customerName, customerName + "@example.com", "123 Main St"),
        Merchant("Acme Store", "acme@example.com", "789 Market St"),
        PaymentMethodFactory::createCreditCard("4111111111111111", customerName, "12/30", "123"),
        amount);
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

} // namespace

class TestReportManager : public QObject {
    Q_OBJECT

private slots:
    void cleanup() {
        ReportManager::getInstance().setPaymentGateway(nullptr);
        ReportManager::getInstance().setRefundManager(nullptr);
    }

    void transactionHistoryListsGatewayTransactions() {
        PaymentGateway gateway;
        auto tx = makeTransaction("Report Alice", 42.5);
        std::string id = tx->getTransactionId();
        gateway.processTransaction(std::move(tx));

        ReportManager& reports = ReportManager::getInstance();
        reports.setPaymentGateway(&gateway);

        std::string report = reports.generateReport(ReportType::TRANSACTION_HISTORY, {});
        QVERIFY(contains(report, id));
        QVERIFY(contains(report, "Report Alice"));
        QVERIFY(contains(report, "Approved"));
    }

    void customerFilterExcludesOtherCustomers() {
        PaymentGateway gateway;
        gateway.processTransaction(makeTransaction("Filter Alice", 10.0));
        gateway.processTransaction(makeTransaction("Filter Bob", 20.0));
        ReportManager::getInstance().setPaymentGateway(&gateway);

        std::string report = ReportManager::getInstance().generateReport(
            ReportType::TRANSACTION_HISTORY, {{"customerId", "Filter Bob"}});
        QVERIFY(contains(report, "Filter Bob"));
        QVERIFY(!contains(report, "Filter Alice"));
    }

    void fraudAlertReportIncludesGatewayAlerts() {
        PaymentGateway gateway;
        auto tx = makeTransaction("Fraud Report Customer", 1500.0);
        std::string id = tx->getTransactionId();
        gateway.processTransaction(std::move(tx));
        ReportManager::getInstance().setPaymentGateway(&gateway);

        std::string report = ReportManager::getInstance().generateReport(ReportType::FRAUD_ALERTS, {});
        QVERIFY(contains(report, id));
    }

    void reportWithoutDataSourcesHasNoRows() {
        std::string report = ReportManager::getInstance().generateReport(ReportType::TRANSACTION_HISTORY, {});
        QVERIFY(!contains(report, "TX-"));
    }
};

QTEST_APPLESS_MAIN(TestReportManager)
#include "tst_reportmanager.moc"
