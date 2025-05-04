#ifndef TRANSACTIONDECORATOR_H
#define TRANSACTIONDECORATOR_H

#include "transaction.h"

/**
 * @class TransactionDecorator
 * @brief Base decorator class for transactions (Decorator Pattern)
 * 
 * This class follows the Decorator Pattern to add functionality to
 * transactions dynamically without affecting other transactions.
 */
class TransactionDecorator : public Transaction {
public:
    /**
     * @brief Constructor
     * @param transaction The transaction to decorate
     */
    explicit TransactionDecorator(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Get the transaction ID
     * @return The transaction ID
     */
    std::string getTransactionId() const override;
    
    /**
     * @brief Get the customer
     * @return Reference to the customer
     */
    const Customer& getCustomer() const override;
    
    /**
     * @brief Get the merchant
     * @return Reference to the merchant
     */
    const Merchant& getMerchant() const override;
    
    /**
     * @brief Get the payment method
     * @return Reference to the payment method
     */
    const PaymentMethod& getPaymentMethod() const override;
    
    /**
     * @brief Get the transaction amount
     * @return The transaction amount
     */
    double getAmount() const override;
    
    /**
     * @brief Get the remaining amount (after refunds)
     * @return The remaining amount
     */
    double getRemainingAmount() const override;
    
    /**
     * @brief Get the refunded amount
     * @return The total refunded amount
     */
    double getRefundedAmount() const override;
    
    /**
     * @brief Get the transaction status
     * @return The transaction status
     */
    TransactionStatus getStatus() const override;
    
    /**
     * @brief Get the timestamp of when the transaction was created
     * @return The timestamp as a string
     */
    std::string getTimestamp() const override;
    
    /**
     * @brief Process the transaction
     * @return True if processing was successful, false otherwise
     */
    bool process() override;
    
    /**
     * @brief Refund the transaction
     * @param amount The amount to refund
     * @return True if refund was successful, false otherwise
     */
    bool refund(double amount) override;
    
    /**
     * @brief Change the transaction state
     * @param state The new state
     */
    void setState(std::unique_ptr<TransactionState> state) override;
    
    /**
     * @brief Add to the refunded amount
     * @param amount The amount to add to the refunded amount
     */
    void addRefundedAmount(double amount) override;
    
protected:
    std::unique_ptr<Transaction> m_transaction;
};

/**
 * @class EncryptedTransaction
 * @brief Decorator that adds encryption to transactions
 */
class EncryptedTransaction : public TransactionDecorator {
public:
    /**
     * @brief Constructor
     * @param transaction The transaction to decorate
     */
    explicit EncryptedTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Process the transaction with encryption
     * @return True if processing was successful, false otherwise
     */
    bool process() override;
    
private:
    /**
     * @brief Encrypt transaction data
     */
    void encryptData();
    
    /**
     * @brief Decrypt transaction data
     */
    void decryptData();
};

/**
 * @class LoggedTransaction
 * @brief Decorator that adds logging to transactions
 */
class LoggedTransaction : public TransactionDecorator {
public:
    /**
     * @brief Constructor
     * @param transaction The transaction to decorate
     * @param logFile Path to the log file
     */
    LoggedTransaction(std::unique_ptr<Transaction> transaction, const std::string& logFile);
    
    /**
     * @brief Process the transaction with logging
     * @return True if processing was successful, false otherwise
     */
    bool process() override;
    
    /**
     * @brief Refund the transaction with logging
     * @param amount The amount to refund
     * @return True if refund was successful, false otherwise
     */
    bool refund(double amount) override;
    
private:
    /**
     * @brief Log a message to the log file
     * @param message The message to log
     */
    void logMessage(const std::string& message);
    
    std::string m_logFile;
};

/**
 * @class TransactionDecoratorFactory
 * @brief Factory for creating decorated transactions (Factory Method Pattern)
 */
class TransactionDecoratorFactory {
public:
    /**
     * @brief Create an encrypted transaction
     * @param transaction The transaction to encrypt
     * @return A unique pointer to the encrypted transaction
     */
    static std::unique_ptr<Transaction> createEncryptedTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Create a logged transaction
     * @param transaction The transaction to log
     * @param logFile Path to the log file
     * @return A unique pointer to the logged transaction
     */
    static std::unique_ptr<Transaction> createLoggedTransaction(std::unique_ptr<Transaction> transaction, const std::string& logFile);
    
    /**
     * @brief Create an encrypted and logged transaction
     * @param transaction The transaction to encrypt and log
     * @param logFile Path to the log file
     * @return A unique pointer to the encrypted and logged transaction
     */
    static std::unique_ptr<Transaction> createEncryptedLoggedTransaction(std::unique_ptr<Transaction> transaction, const std::string& logFile);
};

#endif // TRANSACTIONDECORATOR_H
