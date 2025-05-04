#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>
#include <memory>
#include <string>
#include "customer.h"
#include "merchant.h"
#include "transaction.h"
#include "refund.h"
#include "fraudalert.h"

/**
 * @class DataManager
 * @brief Interface for data persistence operations
 * 
 * This class follows the Interface Segregation Principle by providing
 * a clean interface for data persistence operations.
 */
class DataManager {
public:
    virtual ~DataManager() = default;
    
    /**
     * @brief Initialize the data storage
     * @return True if initialization was successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Save all data to storage
     * @return True if save was successful, false otherwise
     */
    virtual bool saveAll() = 0;
    
    /**
     * @brief Load all data from storage
     * @return True if load was successful, false otherwise
     */
    virtual bool loadAll() = 0;
    
    /**
     * @brief Save a customer to storage
     * @param customer The customer to save
     * @return True if save was successful, false otherwise
     */
    virtual bool saveCustomer(const Customer& customer) = 0;
    
    /**
     * @brief Load all customers from storage
     * @return Vector of customers
     */
    virtual std::vector<Customer> loadCustomers() = 0;
    
    /**
     * @brief Save a merchant to storage
     * @param merchant The merchant to save
     * @return True if save was successful, false otherwise
     */
    virtual bool saveMerchant(const Merchant& merchant) = 0;
    
    /**
     * @brief Load all merchants from storage
     * @return Vector of merchants
     */
    virtual std::vector<Merchant> loadMerchants() = 0;
    
    /**
     * @brief Save a transaction to storage
     * @param transaction The transaction to save
     * @return True if save was successful, false otherwise
     */
    virtual bool saveTransaction(const Transaction& transaction) = 0;
    
    /**
     * @brief Load all transactions from storage
     * @param customers Vector of customers for reference
     * @param merchants Vector of merchants for reference
     * @return Vector of unique pointers to transactions
     */
    virtual std::vector<std::unique_ptr<Transaction>> loadTransactions(
        const std::vector<Customer>& customers,
        const std::vector<Merchant>& merchants) = 0;
    
    /**
     * @brief Save a refund to storage
     * @param refund The refund to save
     * @return True if save was successful, false otherwise
     */
    virtual bool saveRefund(const Refund& refund) = 0;
    
    /**
     * @brief Load all refunds from storage
     * @param transactions Vector of transactions for reference
     * @return Vector of unique pointers to refunds
     */
    virtual std::vector<std::unique_ptr<Refund>> loadRefunds(
        const std::vector<std::unique_ptr<Transaction>>& transactions) = 0;
    
    /**
     * @brief Save a fraud alert to storage
     * @param fraudAlert The fraud alert to save
     * @return True if save was successful, false otherwise
     */
    virtual bool saveFraudAlert(const FraudAlert& fraudAlert) = 0;
    
    /**
     * @brief Load all fraud alerts from storage
     * @param transactions Vector of transactions for reference
     * @return Vector of unique pointers to fraud alerts
     */
    virtual std::vector<std::unique_ptr<FraudAlert>> loadFraudAlerts(
        const std::vector<std::unique_ptr<Transaction>>& transactions) = 0;
};

#endif // DATAMANAGER_H
