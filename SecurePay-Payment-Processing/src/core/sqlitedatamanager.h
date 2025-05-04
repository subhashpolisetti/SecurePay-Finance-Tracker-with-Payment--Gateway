#ifndef SQLITEDATAMANAGER_H
#define SQLITEDATAMANAGER_H

#include <string>
#include <map>
#include <sqlite3.h>
#include "datamanager.h"

/**
 * @class SQLiteDataManager
 * @brief SQLite implementation of the DataManager interface
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * SQLite-specific data persistence operations.
 */
class SQLiteDataManager : public DataManager {
public:
    /**
     * @brief Constructor
     * @param dbPath Path to the SQLite database file
     */
    explicit SQLiteDataManager(const std::string& dbPath);
    
    /**
     * @brief Destructor
     */
    ~SQLiteDataManager() override;
    
    /**
     * @brief Initialize the SQLite database
     * @return True if initialization was successful, false otherwise
     */
    bool initialize() override;
    
    /**
     * @brief Save all data to the SQLite database
     * @return True if save was successful, false otherwise
     */
    bool saveAll() override;
    
    /**
     * @brief Load all data from the SQLite database
     * @return True if load was successful, false otherwise
     */
    bool loadAll() override;
    
    /**
     * @brief Save a customer to the SQLite database
     * @param customer The customer to save
     * @return True if save was successful, false otherwise
     */
    bool saveCustomer(const Customer& customer) override;
    
    /**
     * @brief Load all customers from the SQLite database
     * @return Vector of customers
     */
    std::vector<Customer> loadCustomers() override;
    
    /**
     * @brief Save a merchant to the SQLite database
     * @param merchant The merchant to save
     * @return True if save was successful, false otherwise
     */
    bool saveMerchant(const Merchant& merchant) override;
    
    /**
     * @brief Load all merchants from the SQLite database
     * @return Vector of merchants
     */
    std::vector<Merchant> loadMerchants() override;
    
    /**
     * @brief Save a transaction to the SQLite database
     * @param transaction The transaction to save
     * @return True if save was successful, false otherwise
     */
    bool saveTransaction(const Transaction& transaction) override;
    
    /**
     * @brief Load all transactions from the SQLite database
     * @param customers Vector of customers for reference
     * @param merchants Vector of merchants for reference
     * @return Vector of unique pointers to transactions
     */
    std::vector<std::unique_ptr<Transaction>> loadTransactions(
        const std::vector<Customer>& customers,
        const std::vector<Merchant>& merchants) override;
    
    /**
     * @brief Save a refund to the SQLite database
     * @param refund The refund to save
     * @return True if save was successful, false otherwise
     */
    bool saveRefund(const Refund& refund) override;
    
    /**
     * @brief Load all refunds from the SQLite database
     * @param transactions Vector of transactions for reference
     * @return Vector of unique pointers to refunds
     */
    std::vector<std::unique_ptr<Refund>> loadRefunds(
        const std::vector<std::unique_ptr<Transaction>>& transactions) override;
    
    /**
     * @brief Save a fraud alert to the SQLite database
     * @param fraudAlert The fraud alert to save
     * @return True if save was successful, false otherwise
     */
    bool saveFraudAlert(const FraudAlert& fraudAlert) override;
    
    /**
     * @brief Load all fraud alerts from the SQLite database
     * @param transactions Vector of transactions for reference
     * @return Vector of unique pointers to fraud alerts
     */
    std::vector<std::unique_ptr<FraudAlert>> loadFraudAlerts(
        const std::vector<std::unique_ptr<Transaction>>& transactions) override;
    
    /**
     * @brief Find a customer by name
     * @param name The customer name
     * @param customers Vector of customers to search
     * @return Pointer to the customer, or nullptr if not found
     */
    const Customer* findCustomerByName(const std::string& name, const std::vector<Customer>& customers) const;
    
    /**
     * @brief Find a merchant by name
     * @param name The merchant name
     * @param merchants Vector of merchants to search
     * @return Pointer to the merchant, or nullptr if not found
     */
    const Merchant* findMerchantByName(const std::string& name, const std::vector<Merchant>& merchants) const;
    
    /**
     * @brief Find a transaction by ID
     * @param id The transaction ID
     * @param transactions Vector of transactions to search
     * @return Pointer to the transaction, or nullptr if not found
     */
    const Transaction* findTransactionById(const std::string& id, const std::vector<std::unique_ptr<Transaction>>& transactions) const;
    
    /**
     * @brief Create a payment method from database data
     * @param type The payment method type
     * @param details1 First payment method detail
     * @param details2 Second payment method detail
     * @param details3 Third payment method detail
     * @param details4 Fourth payment method detail
     * @return Unique pointer to the created payment method
     */
    std::unique_ptr<PaymentMethod> createPaymentMethod(
        const std::string& type,
        const std::string& details1,
        const std::string& details2,
        const std::string& details3,
        const std::string& details4) const;
    
private:
    /**
     * @brief Create the database tables if they don't exist
     * @return True if creation was successful, false otherwise
     */
    bool createTables();
    
    /**
     * @brief Execute a SQL statement
     * @param sql The SQL statement to execute
     * @return True if execution was successful, false otherwise
     */
    bool executeSQL(const std::string& sql);
    
    /**
     * @brief Execute a SQL query and process the results
     * @param sql The SQL query to execute
     * @param callback The callback function to process each row
     * @param data User data to pass to the callback
     * @return True if query was successful, false otherwise
     */
    bool executeQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);
    
    
    std::string m_dbPath;
    sqlite3* m_db;
};

#endif // SQLITEDATAMANAGER_H
