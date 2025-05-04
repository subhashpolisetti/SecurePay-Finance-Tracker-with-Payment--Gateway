#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include "transaction.h"
#include "refund.h"
#include "fraudalert.h"

// Forward declarations
class PaymentGateway;
class RefundManager;
class FraudSystem;

/**
 * @enum ReportType
 * @brief Types of reports that can be generated
 */
enum class ReportType {
    TRANSACTION_HISTORY,
    REFUND_HISTORY,
    FRAUD_ALERTS,
    CUSTOMER_SPENDING,
    MERCHANT_EARNINGS,
    DAILY_SUMMARY,
    MONTHLY_SUMMARY
};

/**
 * @enum ExportFormat
 * @brief Formats for exporting reports
 */
enum class ExportFormat {
    CSV,
    JSON
};

/**
 * @class ReportStrategy
 * @brief Abstract base class for report generation strategies (Strategy Pattern)
 * 
 * This class follows the Strategy Pattern to encapsulate different report
 * generation algorithms and make them interchangeable.
 */
class ReportStrategy {
public:
    virtual ~ReportStrategy() = default;
    
    /**
     * @brief Generate report data
     * @param transactions Vector of transactions
     * @param refunds Vector of refunds
     * @param fraudAlerts Vector of fraud alerts
     * @param filterCriteria Optional filter criteria (e.g., customer ID, date range)
     * @return Report data as a string
     */
    virtual std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const = 0;
};

/**
 * @class TransactionHistoryReport
 * @brief Strategy for generating transaction history reports
 */
class TransactionHistoryReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class RefundHistoryReport
 * @brief Strategy for generating refund history reports
 */
class RefundHistoryReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class FraudAlertReport
 * @brief Strategy for generating fraud alert reports
 */
class FraudAlertReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class CustomerSpendingReport
 * @brief Strategy for generating customer spending reports
 */
class CustomerSpendingReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class MerchantEarningsReport
 * @brief Strategy for generating merchant earnings reports
 */
class MerchantEarningsReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class DailySummaryReport
 * @brief Strategy for generating daily summary reports
 */
class DailySummaryReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class MonthlySummaryReport
 * @brief Strategy for generating monthly summary reports
 */
class MonthlySummaryReport : public ReportStrategy {
public:
    std::string generateReport(
        const std::vector<const Transaction*>& transactions,
        const std::vector<const Refund*>& refunds,
        const std::vector<const FraudAlert*>& fraudAlerts,
        const std::map<std::string, std::string>& filterCriteria) const override;
};

/**
 * @class ExportStrategy
 * @brief Abstract base class for export strategies (Strategy Pattern)
 * 
 * This class follows the Strategy Pattern to encapsulate different export
 * algorithms and make them interchangeable.
 */
class ExportStrategy {
public:
    virtual ~ExportStrategy() = default;
    
    /**
     * @brief Export report data to a file
     * @param reportData Report data as a string
     * @param filePath Path to the output file
     * @return True if export was successful, false otherwise
     */
    virtual bool exportToFile(const std::string& reportData, const std::string& filePath) const = 0;
};

/**
 * @class CSVExport
 * @brief Strategy for exporting reports to CSV format
 */
class CSVExport : public ExportStrategy {
public:
    bool exportToFile(const std::string& reportData, const std::string& filePath) const override;
};

/**
 * @class JSONExport
 * @brief Strategy for exporting reports to JSON format
 */
class JSONExport : public ExportStrategy {
public:
    bool exportToFile(const std::string& reportData, const std::string& filePath) const override;
};

/**
 * @class ReportManager
 * @brief Manages report generation and export (Singleton Pattern)
 * 
 * This class follows the Singleton Pattern to ensure a single instance
 * and the Strategy Pattern for report generation and export.
 */
class ReportManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static ReportManager& getInstance();
    
    /**
     * @brief Delete copy constructor
     */
    ReportManager(const ReportManager&) = delete;
    
    /**
     * @brief Delete assignment operator
     */
    ReportManager& operator=(const ReportManager&) = delete;
    
    /**
     * @brief Set the payment gateway
     * @param paymentGateway Pointer to the payment gateway
     */
    void setPaymentGateway(PaymentGateway* paymentGateway);
    
    /**
     * @brief Set the refund manager
     * @param refundManager Pointer to the refund manager
     */
    void setRefundManager(RefundManager* refundManager);
    
    /**
     * @brief Set the fraud system
     * @param fraudSystem Pointer to the fraud system
     */
    void setFraudSystem(FraudSystem* fraudSystem);
    
    /**
     * @brief Generate a report
     * @param reportType Type of report to generate
     * @param filterCriteria Optional filter criteria (e.g., customer ID, date range)
     * @return Report data as a string
     */
    std::string generateReport(
        ReportType reportType,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Export a report to a file
     * @param reportData Report data as a string
     * @param filePath Path to the output file
     * @param format Export format (CSV or JSON)
     * @return True if export was successful, false otherwise
     */
    bool exportReport(
        const std::string& reportData,
        const std::string& filePath,
        ExportFormat format);
    
    /**
     * @brief Generate and export a report in one step
     * @param reportType Type of report to generate
     * @param filePath Path to the output file
     * @param format Export format (CSV or JSON)
     * @param filterCriteria Optional filter criteria (e.g., customer ID, date range)
     * @return True if export was successful, false otherwise
     */
    bool generateAndExportReport(
        ReportType reportType,
        const std::string& filePath,
        ExportFormat format,
        const std::map<std::string, std::string>& filterCriteria = {});
    
private:
    /**
     * @brief Private constructor for singleton
     */
    ReportManager();
    
    /**
     * @brief Get all transactions
     * @return Vector of transactions
     */
    std::vector<const Transaction*> getAllTransactions() const;
    
    /**
     * @brief Get all refunds
     * @return Vector of refunds
     */
    std::vector<const Refund*> getAllRefunds() const;
    
    /**
     * @brief Get all fraud alerts
     * @return Vector of fraud alerts
     */
    std::vector<const FraudAlert*> getAllFraudAlerts() const;
    
    /**
     * @brief Create a report strategy based on report type
     * @param reportType Type of report
     * @return Unique pointer to the report strategy
     */
    std::unique_ptr<ReportStrategy> createReportStrategy(ReportType reportType) const;
    
    /**
     * @brief Create an export strategy based on export format
     * @param format Export format
     * @return Unique pointer to the export strategy
     */
    std::unique_ptr<ExportStrategy> createExportStrategy(ExportFormat format) const;
    
    PaymentGateway* m_paymentGateway;
    RefundManager* m_refundManager;
    FraudSystem* m_fraudSystem;
};

#endif // REPORTMANAGER_H
