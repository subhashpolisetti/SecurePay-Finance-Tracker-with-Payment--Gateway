#include "reportmanager.h"
#include "paymentgateway.h"
#include "refundmanager.h"
#include "fraudsystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

// Helper function to get current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// TransactionHistoryReport implementation
std::string TransactionHistoryReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Transaction History Report\n";
    ss << "=========================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string customerId;
    std::string merchantId;
    std::string startDate;
    std::string endDate;
    
    auto it = filterCriteria.find("customerId");
    if (it != filterCriteria.end()) {
        customerId = it->second;
        ss << "Filter: Customer ID = " << customerId << "\n";
    }
    
    it = filterCriteria.find("merchantId");
    if (it != filterCriteria.end()) {
        merchantId = it->second;
        ss << "Filter: Merchant ID = " << merchantId << "\n";
    }
    
    it = filterCriteria.find("startDate");
    if (it != filterCriteria.end()) {
        startDate = it->second;
        ss << "Filter: Start Date = " << startDate << "\n";
    }
    
    it = filterCriteria.find("endDate");
    if (it != filterCriteria.end()) {
        endDate = it->second;
        ss << "Filter: End Date = " << endDate << "\n";
    }
    
    ss << "\n";
    
    // Create a table with proper spacing and row numbers
    ss << "   ID                      Customer                 Amount                  Payment Method          Status\n";
    ss << "   ----------------------  ----------------------   ----------------------  ----------------------  ----------------------\n";
    
    int rowNumber = 1;
    for (const auto& transaction : transactions) {
        // Apply filters
        if (!customerId.empty() && transaction->getCustomer().getName() != customerId) {
            continue;
        }
        
        if (!merchantId.empty() && transaction->getMerchant().getName() != merchantId) {
            continue;
        }
        
        // TODO: Implement date filtering
        
        // Format the amount with dollar sign
        std::stringstream amountStream;
        amountStream << "$" << std::fixed << std::setprecision(2) << transaction->getAmount();
        
        ss << rowNumber << "  " 
           << std::left << std::setw(22) << transaction->getTransactionId() << "  "
           << std::left << std::setw(22) << transaction->getCustomer().getName() << "  "
           << std::left << std::setw(22) << amountStream.str() << "  "
           << std::left << std::setw(22) << transaction->getPaymentMethod().getType() << "  "
           << std::left << std::setw(22) << Transaction::statusToString(transaction->getStatus()) << "\n";
        
        rowNumber++;
    }
    
    return ss.str();
}

// RefundHistoryReport implementation
std::string RefundHistoryReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Refund History Report\n";
    ss << "====================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string customerId;
    std::string merchantId;
    std::string startDate;
    std::string endDate;
    
    auto it = filterCriteria.find("customerId");
    if (it != filterCriteria.end()) {
        customerId = it->second;
        ss << "Filter: Customer ID = " << customerId << "\n";
    }
    
    it = filterCriteria.find("merchantId");
    if (it != filterCriteria.end()) {
        merchantId = it->second;
        ss << "Filter: Merchant ID = " << merchantId << "\n";
    }
    
    it = filterCriteria.find("startDate");
    if (it != filterCriteria.end()) {
        startDate = it->second;
        ss << "Filter: Start Date = " << startDate << "\n";
    }
    
    it = filterCriteria.find("endDate");
    if (it != filterCriteria.end()) {
        endDate = it->second;
        ss << "Filter: End Date = " << endDate << "\n";
    }
    
    ss << "\n";
    ss << "ID\tDate\tTransaction ID\tCustomer\tMerchant\tAmount\tReason\n";
    
    for (const auto& refund : refunds) {
        const Transaction& transaction = refund->getTransaction();
        
        // Apply filters
        if (!customerId.empty() && transaction.getCustomer().getName() != customerId) {
            continue;
        }
        
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        // TODO: Implement date filtering
        
        ss << refund->getRefundId() << "\t"
           << refund->getTimestamp() << "\t"
           << transaction.getTransactionId() << "\t"
           << transaction.getCustomer().getName() << "\t"
           << transaction.getMerchant().getName() << "\t"
           << refund->getAmount() << "\t"
           << refund->getReason() << "\n";
    }
    
    return ss.str();
}

// FraudAlertReport implementation
std::string FraudAlertReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Fraud Alert Report\n";
    ss << "=================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string riskLevel;
    std::string reviewed;
    std::string startDate;
    std::string endDate;
    
    auto it = filterCriteria.find("riskLevel");
    if (it != filterCriteria.end()) {
        riskLevel = it->second;
        ss << "Filter: Risk Level = " << riskLevel << "\n";
    }
    
    it = filterCriteria.find("reviewed");
    if (it != filterCriteria.end()) {
        reviewed = it->second;
        ss << "Filter: Reviewed = " << reviewed << "\n";
    }
    
    it = filterCriteria.find("startDate");
    if (it != filterCriteria.end()) {
        startDate = it->second;
        ss << "Filter: Start Date = " << startDate << "\n";
    }
    
    it = filterCriteria.find("endDate");
    if (it != filterCriteria.end()) {
        endDate = it->second;
        ss << "Filter: End Date = " << endDate << "\n";
    }
    
    ss << "\n";
    ss << "ID\tDate\tTransaction ID\tCustomer\tMerchant\tAmount\tRisk Level\tDescription\tReviewed\n";
    
    for (const auto& alert : fraudAlerts) {
        const Transaction& transaction = alert->getTransaction();
        
        // Apply filters
        if (!riskLevel.empty()) {
            std::string alertRiskLevel = FraudSystem::riskLevelToString(alert->getRiskLevel());
            if (alertRiskLevel != riskLevel) {
                continue;
            }
        }
        
        if (!reviewed.empty()) {
            bool isReviewed = alert->isReviewed();
            if ((reviewed == "true" && !isReviewed) || (reviewed == "false" && isReviewed)) {
                continue;
            }
        }
        
        // TODO: Implement date filtering
        
        ss << alert->getAlertId() << "\t"
           << alert->getTimestamp() << "\t"
           << transaction.getTransactionId() << "\t"
           << transaction.getCustomer().getName() << "\t"
           << transaction.getMerchant().getName() << "\t"
           << transaction.getAmount() << "\t"
           << FraudSystem::riskLevelToString(alert->getRiskLevel()) << "\t"
           << alert->getDescription() << "\t"
           << (alert->isReviewed() ? "Yes" : "No") << "\n";
    }
    
    return ss.str();
}

// CustomerSpendingReport implementation
std::string CustomerSpendingReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Customer Spending Report\n";
    ss << "=======================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string customerId;
    std::string startDate;
    std::string endDate;
    
    auto it = filterCriteria.find("customerId");
    if (it != filterCriteria.end()) {
        customerId = it->second;
        ss << "Filter: Customer ID = " << customerId << "\n";
    }
    
    it = filterCriteria.find("startDate");
    if (it != filterCriteria.end()) {
        startDate = it->second;
        ss << "Filter: Start Date = " << startDate << "\n";
    }
    
    it = filterCriteria.find("endDate");
    if (it != filterCriteria.end()) {
        endDate = it->second;
        ss << "Filter: End Date = " << endDate << "\n";
    }
    
    ss << "\n";
    
    // Group transactions by customer
    std::map<std::string, std::vector<const Transaction*>> customerTransactions;
    std::map<std::string, double> customerTotals;
    
    for (const auto& transaction : transactions) {
        // Apply filters
        if (!customerId.empty() && transaction->getCustomer().getName() != customerId) {
            continue;
        }
        
        // TODO: Implement date filtering
        
        std::string customerName = transaction->getCustomer().getName();
        customerTransactions[customerName].push_back(transaction);
        
        // Only count approved transactions
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            customerTotals[customerName] += transaction->getRemainingAmount();
        }
    }
    
    // Output customer spending summary
    ss << "   ID                      Customer                 Amount                  Payment Method          Status\n";
    ss << "   ----------------------  ----------------------   ----------------------  ----------------------  ----------------------\n";
    
    // If a specific customer ID is provided, show their transactions
    if (!customerId.empty()) {
        int rowNumber = 1;
        for (const auto& transaction : customerTransactions[customerId]) {
            // Format the amount with dollar sign
            std::stringstream amountStream;
            amountStream << "$" << std::fixed << std::setprecision(2) << transaction->getAmount();
            
            ss << rowNumber << "  " 
               << std::left << std::setw(22) << transaction->getTransactionId() << "  "
               << std::left << std::setw(22) << transaction->getCustomer().getName() << "  "
               << std::left << std::setw(22) << amountStream.str() << "  "
               << std::left << std::setw(22) << transaction->getPaymentMethod().getType() << "  "
               << std::left << std::setw(22) << Transaction::statusToString(transaction->getStatus()) << "\n";
            
            rowNumber++;
        }
    } else {
        // If no specific customer, show a summary for each customer
        int rowNumber = 1;
        for (const auto& pair : customerTotals) {
            std::string customerName = pair.first;
            double totalSpending = pair.second;
            
            // Format the amount with dollar sign
            std::stringstream amountStream;
            amountStream << "$" << std::fixed << std::setprecision(2) << totalSpending;
            
            ss << rowNumber << "  " 
               << std::left << std::setw(22) << "SUMMARY" << "  "
               << std::left << std::setw(22) << customerName << "  "
               << std::left << std::setw(22) << amountStream.str() << "  "
               << std::left << std::setw(22) << "All Methods" << "  "
               << std::left << std::setw(22) << "Total: " + std::to_string(customerTransactions[customerName].size()) + " transactions" << "\n";
            
            rowNumber++;
        }
    }
    
    return ss.str();
}

// MerchantEarningsReport implementation
std::string MerchantEarningsReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Merchant Earnings Report\n";
    ss << "=======================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string merchantId;
    std::string startDate;
    std::string endDate;
    
    auto it = filterCriteria.find("merchantId");
    if (it != filterCriteria.end()) {
        merchantId = it->second;
        ss << "Filter: Merchant ID = " << merchantId << "\n";
    }
    
    it = filterCriteria.find("startDate");
    if (it != filterCriteria.end()) {
        startDate = it->second;
        ss << "Filter: Start Date = " << startDate << "\n";
    }
    
    it = filterCriteria.find("endDate");
    if (it != filterCriteria.end()) {
        endDate = it->second;
        ss << "Filter: End Date = " << endDate << "\n";
    }
    
    ss << "\n";
    
    // Group transactions by merchant
    std::map<std::string, std::vector<const Transaction*>> merchantTransactions;
    std::map<std::string, double> merchantTotals;
    std::map<std::string, double> merchantRefunds;
    
    for (const auto& transaction : transactions) {
        // Apply filters
        if (!merchantId.empty() && transaction->getMerchant().getName() != merchantId) {
            continue;
        }
        
        // TODO: Implement date filtering
        
        std::string merchantName = transaction->getMerchant().getName();
        merchantTransactions[merchantName].push_back(transaction);
        
        // Only count approved transactions
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            merchantTotals[merchantName] += transaction->getRemainingAmount();
        }
    }
    
    // Calculate refund totals
    for (const auto& refund : refunds) {
        const Transaction& transaction = refund->getTransaction();
        
        // Apply filters
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        // TODO: Implement date filtering
        
        std::string merchantName = transaction.getMerchant().getName();
        merchantRefunds[merchantName] += refund->getAmount();
    }
    
    // Output merchant earnings summary
    ss << "Merchant\tGross Earnings\tRefunds\tNet Earnings\tTransaction Count\n";
    
    for (const auto& pair : merchantTotals) {
        std::string merchantName = pair.first;
        double grossEarnings = pair.second;
        double refunds = merchantRefunds[merchantName];
        double netEarnings = grossEarnings - refunds;
        
        ss << merchantName << "\t"
           << grossEarnings << "\t"
           << refunds << "\t"
           << netEarnings << "\t"
           << merchantTransactions[merchantName].size() << "\n";
    }
    
    return ss.str();
}

// DailySummaryReport implementation
std::string DailySummaryReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Daily Summary Report\n";
    ss << "===================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string date;
    std::string merchantId;
    
    auto it = filterCriteria.find("date");
    if (it != filterCriteria.end()) {
        date = it->second;
        ss << "Filter: Date = " << date << "\n";
    }
    
    it = filterCriteria.find("merchantId");
    if (it != filterCriteria.end()) {
        merchantId = it->second;
        ss << "Filter: Merchant ID = " << merchantId << "\n";
    }
    
    ss << "\n";
    
    // Group transactions by date
    std::map<std::string, std::vector<const Transaction*>> dateTransactions;
    std::map<std::string, double> dateTotals;
    std::map<std::string, double> dateRefunds;
    std::map<std::string, int> dateFraudAlerts;
    
    for (const auto& transaction : transactions) {
        // Apply filters
        if (!merchantId.empty() && transaction->getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string transactionDate = transaction->getTimestamp().substr(0, 10); // Extract YYYY-MM-DD
        
        if (!date.empty() && transactionDate != date) {
            continue;
        }
        
        dateTransactions[transactionDate].push_back(transaction);
        
        // Only count approved transactions
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            dateTotals[transactionDate] += transaction->getRemainingAmount();
        }
    }
    
    // Calculate refund totals
    for (const auto& refund : refunds) {
        const Transaction& transaction = refund->getTransaction();
        
        // Apply filters
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string refundDate = refund->getTimestamp().substr(0, 10); // Extract YYYY-MM-DD
        
        if (!date.empty() && refundDate != date) {
            continue;
        }
        
        dateRefunds[refundDate] += refund->getAmount();
    }
    
    // Count fraud alerts
    for (const auto& alert : fraudAlerts) {
        const Transaction& transaction = alert->getTransaction();
        
        // Apply filters
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string alertDate = alert->getTimestamp().substr(0, 10); // Extract YYYY-MM-DD
        
        if (!date.empty() && alertDate != date) {
            continue;
        }
        
        dateFraudAlerts[alertDate]++;
    }
    
    // Output daily summary
    ss << "Date\tTransaction Count\tGross Amount\tRefunds\tNet Amount\tFraud Alerts\n";
    
    for (const auto& pair : dateTransactions) {
        std::string dateStr = pair.first;
        double grossAmount = dateTotals[dateStr];
        double refundAmount = dateRefunds[dateStr];
        double netAmount = grossAmount - refundAmount;
        int fraudAlertCount = dateFraudAlerts[dateStr];
        
        ss << dateStr << "\t"
           << pair.second.size() << "\t"
           << grossAmount << "\t"
           << refundAmount << "\t"
           << netAmount << "\t"
           << fraudAlertCount << "\n";
    }
    
    return ss.str();
}

// MonthlySummaryReport implementation
std::string MonthlySummaryReport::generateReport(
    const std::vector<const Transaction*>& transactions,
    const std::vector<const Refund*>& refunds,
    const std::vector<const FraudAlert*>& fraudAlerts,
    const std::map<std::string, std::string>& filterCriteria) const {
    
    std::stringstream ss;
    ss << "Monthly Summary Report\n";
    ss << "=====================\n\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Apply filters if any
    std::string month;
    std::string year;
    std::string merchantId;
    
    auto it = filterCriteria.find("month");
    if (it != filterCriteria.end()) {
        month = it->second;
        ss << "Filter: Month = " << month << "\n";
    }
    
    it = filterCriteria.find("year");
    if (it != filterCriteria.end()) {
        year = it->second;
        ss << "Filter: Year = " << year << "\n";
    }
    
    it = filterCriteria.find("merchantId");
    if (it != filterCriteria.end()) {
        merchantId = it->second;
        ss << "Filter: Merchant ID = " << merchantId << "\n";
    }
    
    ss << "\n";
    
    // Group transactions by month
    std::map<std::string, std::vector<const Transaction*>> monthTransactions;
    std::map<std::string, double> monthTotals;
    std::map<std::string, double> monthRefunds;
    std::map<std::string, int> monthFraudAlerts;
    
    for (const auto& transaction : transactions) {
        // Apply filters
        if (!merchantId.empty() && transaction->getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string transactionDate = transaction->getTimestamp().substr(0, 7); // Extract YYYY-MM
        
        if (!year.empty() && transactionDate.substr(0, 4) != year) {
            continue;
        }
        
        if (!month.empty() && transactionDate.substr(5, 2) != month) {
            continue;
        }
        
        monthTransactions[transactionDate].push_back(transaction);
        
        // Only count approved transactions
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            monthTotals[transactionDate] += transaction->getRemainingAmount();
        }
    }
    
    // Calculate refund totals
    for (const auto& refund : refunds) {
        const Transaction& transaction = refund->getTransaction();
        
        // Apply filters
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string refundDate = refund->getTimestamp().substr(0, 7); // Extract YYYY-MM
        
        if (!year.empty() && refundDate.substr(0, 4) != year) {
            continue;
        }
        
        if (!month.empty() && refundDate.substr(5, 2) != month) {
            continue;
        }
        
        monthRefunds[refundDate] += refund->getAmount();
    }
    
    // Count fraud alerts
    for (const auto& alert : fraudAlerts) {
        const Transaction& transaction = alert->getTransaction();
        
        // Apply filters
        if (!merchantId.empty() && transaction.getMerchant().getName() != merchantId) {
            continue;
        }
        
        std::string alertDate = alert->getTimestamp().substr(0, 7); // Extract YYYY-MM
        
        if (!year.empty() && alertDate.substr(0, 4) != year) {
            continue;
        }
        
        if (!month.empty() && alertDate.substr(5, 2) != month) {
            continue;
        }
        
        monthFraudAlerts[alertDate]++;
    }
    
    // Output monthly summary
    ss << "Month\tTransaction Count\tGross Amount\tRefunds\tNet Amount\tFraud Alerts\n";
    
    for (const auto& pair : monthTransactions) {
        std::string monthStr = pair.first;
        double grossAmount = monthTotals[monthStr];
        double refundAmount = monthRefunds[monthStr];
        double netAmount = grossAmount - refundAmount;
        int fraudAlertCount = monthFraudAlerts[monthStr];
        
        ss << monthStr << "\t"
           << pair.second.size() << "\t"
           << grossAmount << "\t"
           << refundAmount << "\t"
           << netAmount << "\t"
           << fraudAlertCount << "\n";
    }
    
    return ss.str();
}

// CSVExport implementation
bool CSVExport::exportToFile(const std::string& reportData, const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return false;
    }
    
    // For CSV export, we'll create a simple table with the transaction data
    std::stringstream outputSS;
    
    // Write the header row for Excel
    outputSS << "ID\tCustomer\tAmount\tPayment Method\tStatus\n";
    
    // Parse the report data to extract transaction information
    std::istringstream iss(reportData);
    std::string line;
    
    // Skip header lines until we find the table header with row numbers
    while (std::getline(iss, line)) {
        if (line.find("ID") != std::string::npos && 
            line.find("Customer") != std::string::npos && 
            line.find("Amount") != std::string::npos) {
            break;
        }
    }
    
    // Skip the separator line
    std::getline(iss, line);
    
    // Process data rows
    while (std::getline(iss, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Check if this is a data line (starts with a number)
        if (std::isdigit(line[0])) {
            // Extract the row number
            size_t pos = line.find_first_of(" \t");
            std::string rowNum = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            // Split the remaining line by double spaces
            std::vector<std::string> fields;
            size_t start = 0;
            size_t end = 0;
            
            while ((end = line.find("  ", start)) != std::string::npos) {
                std::string field = line.substr(start, end - start);
                if (!field.empty()) {
                    fields.push_back(field);
                }
                start = end + 2;
                
                // Skip additional spaces
                while (start < line.length() && line[start] == ' ') {
                    start++;
                }
            }
            
            // Add the last field
            if (start < line.length()) {
                fields.push_back(line.substr(start));
            }
            
            // If we have all the fields, write them to the output
            if (fields.size() >= 5) {
                outputSS << fields[0] << "\t"  // ID
                         << fields[1] << "\t"  // Customer
                         << fields[2] << "\t"  // Amount
                         << fields[3] << "\t"  // Payment Method
                         << fields[4] << "\n"; // Status
            }
        }
    }
    
    file << outputSS.str();
    file.close();
    
    std::cout << "Report exported to CSV file: " << filePath << std::endl;
    return true;
}

// JSONExport implementation
bool JSONExport::exportToFile(const std::string& reportData, const std::string& filePath) const {
    // For simplicity, we'll just convert the CSV data to a basic JSON format
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return false;
    }
    
    // For JSON export, we'll create a JSON array of transaction objects
    std::vector<std::string> headers = {"ID", "Customer", "Amount", "Payment Method", "Status"};
    std::vector<std::vector<std::string>> rows;
    
    // Parse the report data to extract transaction information
    std::istringstream iss(reportData);
    std::string line;
    
    // Skip header lines until we find the table header with row numbers
    while (std::getline(iss, line)) {
        if (line.find("ID") != std::string::npos && 
            line.find("Customer") != std::string::npos && 
            line.find("Amount") != std::string::npos) {
            break;
        }
    }
    
    // Skip the separator line
    std::getline(iss, line);
    
    // Process data rows
    while (std::getline(iss, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Check if this is a data line (starts with a number)
        if (std::isdigit(line[0])) {
            // Extract the row number
            size_t pos = line.find_first_of(" \t");
            std::string rowNum = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            // Split the remaining line by double spaces
            std::vector<std::string> fields;
            size_t start = 0;
            size_t end = 0;
            
            while ((end = line.find("  ", start)) != std::string::npos) {
                std::string field = line.substr(start, end - start);
                if (!field.empty()) {
                    fields.push_back(field);
                }
                start = end + 2;
                
                // Skip additional spaces
                while (start < line.length() && line[start] == ' ') {
                    start++;
                }
            }
            
            // Add the last field
            if (start < line.length()) {
                fields.push_back(line.substr(start));
            }
            
            // If we have all the fields, add them to the rows
            if (fields.size() >= 5) {
                std::vector<std::string> row = {
                    fields[0],  // ID
                    fields[1],  // Customer
                    fields[2],  // Amount
                    fields[3],  // Payment Method
                    fields[4]   // Status
                };
                rows.push_back(row);
            }
        }
    }
    
    // Write JSON
    file << "[\n";
    for (size_t i = 0; i < rows.size(); ++i) {
        file << "  {\n";
        for (size_t j = 0; j < headers.size() && j < rows[i].size(); ++j) {
            file << "    \"" << headers[j] << "\": \"" << rows[i][j] << "\"";
            if (j < headers.size() - 1 && j < rows[i].size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        file << "  }";
        if (i < rows.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "]\n";
    
    file.close();
    
    std::cout << "Report exported to JSON file: " << filePath << std::endl;
    return true;
}

// ReportManager implementation
ReportManager& ReportManager::getInstance() {
    static ReportManager instance;
    return instance;
}

ReportManager::ReportManager()
    : m_paymentGateway(nullptr), m_refundManager(nullptr), m_fraudSystem(nullptr) {
    std::cout << "ReportManager initialized" << std::endl;
}

void ReportManager::setPaymentGateway(PaymentGateway* paymentGateway) {
    m_paymentGateway = paymentGateway;
}

void ReportManager::setRefundManager(RefundManager* refundManager) {
    m_refundManager = refundManager;
}

void ReportManager::setFraudSystem(FraudSystem* fraudSystem) {
    m_fraudSystem = fraudSystem;
}

std::string ReportManager::generateReport(
    ReportType reportType,
    const std::map<std::string, std::string>& filterCriteria) {
    
    auto transactions = getAllTransactions();
    auto refunds = getAllRefunds();
    auto fraudAlerts = getAllFraudAlerts();
    
    auto strategy = createReportStrategy(reportType);
    return strategy->generateReport(transactions, refunds, fraudAlerts, filterCriteria);
}

bool ReportManager::exportReport(
    const std::string& reportData,
    const std::string& filePath,
    ExportFormat format) {
    
    auto strategy = createExportStrategy(format);
    return strategy->exportToFile(reportData, filePath);
}

bool ReportManager::generateAndExportReport(
    ReportType reportType,
    const std::string& filePath,
    ExportFormat format,
    const std::map<std::string, std::string>& filterCriteria) {
    
    std::string reportData = generateReport(reportType, filterCriteria);
    return exportReport(reportData, filePath, format);
}

std::vector<const Transaction*> ReportManager::getAllTransactions() const {
    std::vector<const Transaction*> result;
    
    if (m_paymentGateway) {
        for (const auto& transaction : m_paymentGateway->getTransactions()) {
            result.push_back(transaction.get());
        }
    }
    
    return result;
}

std::vector<const Refund*> ReportManager::getAllRefunds() const {
    std::vector<const Refund*> result;
    
    if (m_refundManager) {
        for (const auto& refund : m_refundManager->getRefunds()) {
            result.push_back(refund.get());
        }
    }
    
    return result;
}

std::vector<const FraudAlert*> ReportManager::getAllFraudAlerts() const {
    std::vector<const FraudAlert*> result;
    
    // TODO: Implement once FraudSystem is updated to store alerts
    
    return result;
}

std::unique_ptr<ReportStrategy> ReportManager::createReportStrategy(ReportType reportType) const {
    switch (reportType) {
        case ReportType::TRANSACTION_HISTORY:
            return std::make_unique<TransactionHistoryReport>();
        case ReportType::REFUND_HISTORY:
            return std::make_unique<RefundHistoryReport>();
        case ReportType::FRAUD_ALERTS:
            return std::make_unique<FraudAlertReport>();
        case ReportType::CUSTOMER_SPENDING:
            return std::make_unique<CustomerSpendingReport>();
        case ReportType::MERCHANT_EARNINGS:
            return std::make_unique<MerchantEarningsReport>();
        case ReportType::DAILY_SUMMARY:
            return std::make_unique<DailySummaryReport>();
        case ReportType::MONTHLY_SUMMARY:
            return std::make_unique<MonthlySummaryReport>();
        default:
            return std::make_unique<TransactionHistoryReport>();
    }
}

std::unique_ptr<ExportStrategy> ReportManager::createExportStrategy(ExportFormat format) const {
    switch (format) {
        case ExportFormat::CSV:
            return std::make_unique<CSVExport>();
        case ExportFormat::JSON:
            return std::make_unique<JSONExport>();
        default:
            return std::make_unique<CSVExport>();
    }
}
