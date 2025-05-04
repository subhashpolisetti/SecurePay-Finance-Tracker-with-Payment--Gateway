#ifndef LAZYREPORT_H
#define LAZYREPORT_H

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "reportmanager.h"

/**
 * @class LazyReport
 * @brief Implements lazy evaluation for report generation
 * 
 * This class follows the Lazy Evaluation principle by deferring the
 * expensive operation of report generation until the data is actually needed.
 */
class LazyReport {
public:
    /**
     * @brief Constructor
     * @param reportManager Reference to the report manager
     * @param reportType Type of report to generate
     * @param filterCriteria Filter criteria for the report
     */
    LazyReport(
        ReportManager& reportManager,
        ReportType reportType,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Get the report data
     * @return The report data as a string
     */
    const std::string& getData() const;
    
    /**
     * @brief Get the report type
     * @return The report type
     */
    ReportType getReportType() const;
    
    /**
     * @brief Get the filter criteria
     * @return The filter criteria
     */
    const std::map<std::string, std::string>& getFilterCriteria() const;
    
    /**
     * @brief Export the report to a file
     * @param filePath Path to the output file
     * @param format Export format (CSV or JSON)
     * @return True if export was successful, false otherwise
     */
    bool exportToFile(const std::string& filePath, ExportFormat format) const;
    
private:
    ReportManager& m_reportManager;
    ReportType m_reportType;
    std::map<std::string, std::string> m_filterCriteria;
    mutable std::string m_data;
    mutable bool m_dataGenerated;
    
    /**
     * @brief Generate the report data if not already generated
     */
    void generateDataIfNeeded() const;
};

/**
 * @class LazyReportFactory
 * @brief Factory for creating lazy reports
 */
class LazyReportFactory {
public:
    /**
     * @brief Create a transaction history report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createTransactionHistoryReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a refund history report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createRefundHistoryReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a fraud alert report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createFraudAlertReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a customer spending report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createCustomerSpendingReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a merchant earnings report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createMerchantEarningsReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a daily summary report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createDailySummaryReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
    
    /**
     * @brief Create a monthly summary report
     * @param reportManager Reference to the report manager
     * @param filterCriteria Filter criteria for the report
     * @return Unique pointer to the lazy report
     */
    static std::unique_ptr<LazyReport> createMonthlySummaryReport(
        ReportManager& reportManager,
        const std::map<std::string, std::string>& filterCriteria = {});
};

#endif // LAZYREPORT_H
