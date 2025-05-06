#ifndef PAYMENTGATEWAY_H
#define PAYMENTGATEWAY_H

#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include "transaction.h"
#include "fraudsystem.h"
#include "bank.h"

// Observer interface for transaction updates (Observer pattern)
class TransactionObserver {
public:
    virtual ~TransactionObserver() = default;
    virtual void onTransactionUpdated(const Transaction& transaction) = 0;
};

/**
 * @class PaymentGateway
 * @brief Class for processing payments
 * 
 * This class follows the Observer Pattern to notify observers of transaction updates
 * and provides methods for processing transactions.
 */
class PaymentGateway {
public:
    /**
     * @brief Constructor
     */
    PaymentGateway();
    
    /**
     * @brief Process a transaction
     * @param transaction The transaction to process
     */
    void processTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Store a transaction without processing it
     * @param transaction The transaction to store
     */
    void storeTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Process a transaction with an idempotency key
     * @param transaction The transaction to process
     * @param idempotencyKey The idempotency key
     * @return The transaction ID
     */
    std::string processTransactionWithIdempotencyKey(
        std::unique_ptr<Transaction> transaction,
        const std::string& idempotencyKey);
    
    /**
     * @brief Authorize a transaction (hold funds without capturing)
     * @param transaction The transaction to authorize
     * @return True if authorization was successful, false otherwise
     */
    bool authorizeTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Capture a previously authorized transaction
     * @param transactionId The ID of the transaction to capture
     * @param amount The amount to capture (0 for full amount)
     * @return True if capture was successful, false otherwise
     */
    bool captureTransaction(const std::string& transactionId, double amount = 0);
    
    /**
     * @brief Void a previously authorized transaction
     * @param transactionId The ID of the transaction to void
     * @return True if void was successful, false otherwise
     */
    bool voidTransaction(const std::string& transactionId);
    
    /**
     * @brief Get all transactions
     * @return Vector of transactions
     */
    const std::vector<std::unique_ptr<Transaction>>& getTransactions() const;
    
    /**
     * @brief Find a transaction by ID
     * @param transactionId The transaction ID
     * @return Pointer to the transaction, or nullptr if not found
     */
    Transaction* findTransaction(const std::string& transactionId);
    
    /**
     * @brief Find a transaction by idempotency key
     * @param idempotencyKey The idempotency key
     * @return Pointer to the transaction, or nullptr if not found
     */
    Transaction* findTransactionByIdempotencyKey(const std::string& idempotencyKey);
    
    /**
     * @brief Add an observer for transaction updates
     * @param observer The observer to add
     */
    void addObserver(TransactionObserver* observer);
    
    /**
     * @brief Remove an observer
     * @param observer The observer to remove
     */
    void removeObserver(TransactionObserver* observer);
    
private:
    /**
     * @brief Vector of transactions
     */
    std::vector<std::unique_ptr<Transaction>> m_transactions;
    
    /**
     * @brief Map of idempotency keys to transaction IDs
     */
    std::map<std::string, std::string> m_idempotencyKeys;
    
    /**
     * @brief Vector of observers
     */
    std::vector<TransactionObserver*> m_observers;
    
    /**
     * @brief Notify observers of a transaction update
     * @param transaction The updated transaction
     */
    void notifyObservers(const Transaction& transaction);
    
    /**
     * @brief Encrypt transaction data
     * @param transaction The transaction to encrypt
     */
    void encryptTransactionData(const Transaction& transaction);
};

#endif // PAYMENTGATEWAY_H
