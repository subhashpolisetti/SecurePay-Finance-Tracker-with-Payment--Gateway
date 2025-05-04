#include "lazyreport.h"
#include <iostream>

LazyReport::LazyReport(
    ReportManager& reportManager,
    ReportType reportType,
    const std::map<std::string, std::string>& filterCriteria)
    : m_reportManager(reportManager),
      m_reportType(reportType),
      m_filterCriteria(filterCriteria),
      m_dataGenerated(false) {
}

const std::string& LazyReport::getData() const {
    generateDataIfNeeded();
    return m_data;
}

ReportType LazyReport::getReportType() const {
    return m_reportType;
}

const std::map<std::string, std::string>& LazyReport::getFilterCriteria() const {
    return m_filterCriteria;
}

bool LazyReport::exportToFile(const std::string& filePath, ExportFormat format) const {
    generateDataIfNeeded();
    return m_reportManager.exportReport(m_data, filePath, format);
}

void LazyReport::generateDataIfNeeded() const {
    if (!m_dataGenerated) {
        std::cout << "Generating report data on demand..." << std::endl;
        m_data = m_reportManager.generateReport(m_reportType, m_filterCriteria);
        m_dataGenerated = true;
    }
}

// LazyReportFactory implementation
std::unique_ptr<LazyReport> LazyReportFactory::createTransactionHistoryReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::TRANSACTION_HISTORY, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createRefundHistoryReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::REFUND_HISTORY, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createFraudAlertReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::FRAUD_ALERTS, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createCustomerSpendingReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::CUSTOMER_SPENDING, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createMerchantEarningsReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::MERCHANT_EARNINGS, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createDailySummaryReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::DAILY_SUMMARY, filterCriteria);
}

std::unique_ptr<LazyReport> LazyReportFactory::createMonthlySummaryReport(
    ReportManager& reportManager,
    const std::map<std::string, std::string>& filterCriteria) {
    return std::make_unique<LazyReport>(
        reportManager, ReportType::MONTHLY_SUMMARY, filterCriteria);
}
