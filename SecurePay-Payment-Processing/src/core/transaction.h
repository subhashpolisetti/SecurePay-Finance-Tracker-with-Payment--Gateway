#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include "customer.h"
#include "merchant.h"
#include "paymentmethod.h"

// Forward declarations
class Transaction;
class TransactionState;

/**
 * @enum TransactionStatus
 * @brief Represents the possible states of a transaction
 */
enum class TransactionStatus {
    PENDING,
    APPROVED,
    DECLINED,
    FLAGGED_FOR_REVIEW,
    REFUNDED,
    PARTIALLY_REFUNDED
};

/**
 * @class TransactionState
 * @brief Abstract base class for transaction states (State Pattern)
 * 
 * This class follows the State Pattern to encapsulate state-specific behavior
 * and allow the transaction to change its behavior when its state changes.
 */
class TransactionState {
public:
    virtual ~TransactionState() = default;
    
    /**
     * @brief Process the transaction in its current state
     * @param transaction The transaction to process
     * @return True if processing was successful, false otherwise
     */
    virtual bool process(Transaction& transaction) = 0;
    
    /**
     * @brief Refund the transaction in its current state
     * @param transaction The transaction to refund
     * @param amount The amount to refund
     * @return True if refund was successful, false otherwise
     */
    virtual bool refund(Transaction& transaction, double amount) = 0;
    
    /**
     * @brief Get the status of the transaction in its current state
     * @return The transaction status
     */
    virtual TransactionStatus getStatus() const = 0;
    
    /**
     * @brief Get a string representation of the state
     * @return The state as a string
     */
    virtual std::string toString() const = 0;
};

/**
 * @class Transaction
 * @brief Represents a payment transaction
 * 
 * This class follows the State Pattern to manage transaction states
 * and the Single Responsibility Principle by focusing on transaction data
 * and operations.
 */
class Transaction {
public:
    /**
     * @brief Constructor for a transaction
     * @param customer The customer making the payment
     * @param merchant The merchant receiving the payment
     * @param paymentMethod The payment method used
     * @param amount The transaction amount
     */
    Transaction(const Customer& customer, const Merchant& merchant, 
                std::unique_ptr<PaymentMethod> paymentMethod, double amount);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Transaction() = default;
    
    /**
     * @brief Get the transaction ID
     * @return The unique transaction ID
     */
    virtual std::string getTransactionId() const;
    
    /**
     * @brief Get the customer
     * @return Reference to the customer
     */
    virtual const Customer& getCustomer() const;
    
    /**
     * @brief Get the merchant
     * @return Reference to the merchant
     */
    virtual const Merchant& getMerchant() const;
    
    /**
     * @brief Get the payment method
     * @return Reference to the payment method
     */
    virtual const PaymentMethod& getPaymentMethod() const;
    
    /**
     * @brief Get the transaction amount
     * @return The transaction amount
     */
    virtual double getAmount() const;
    
    /**
     * @brief Get the remaining amount (after refunds)
     * @return The remaining amount
     */
    virtual double getRemainingAmount() const;
    
    /**
     * @brief Get the refunded amount
     * @return The total refunded amount
     */
    virtual double getRefundedAmount() const;
    
    /**
     * @brief Get the transaction status
     * @return The transaction status
     */
    virtual TransactionStatus getStatus() const;
    
    /**
     * @brief Get the timestamp of when the transaction was created
     * @return The timestamp as a string
     */
    virtual std::string getTimestamp() const;
    
    /**
     * @brief Process the transaction
     * @return True if processing was successful, false otherwise
     */
    virtual bool process();
    
    /**
     * @brief Refund the transaction
     * @param amount The amount to refund
     * @return True if refund was successful, false otherwise
     */
    virtual bool refund(double amount);
    
    /**
     * @brief Change the transaction state
     * @param state The new state
     */
    virtual void setState(std::unique_ptr<TransactionState> state);
    
    /**
     * @brief Add to the refunded amount
     * @param amount The amount to add to the refunded amount
     */
    virtual void addRefundedAmount(double amount);
    
    /**
     * @brief Convert a transaction status to a string
     * @param status The transaction status
     * @return The status as a string
     */
    static std::string statusToString(TransactionStatus status);
    
private:
    std::string m_transactionId;
    Customer m_customer;
    Merchant m_merchant;
    std::unique_ptr<PaymentMethod> m_paymentMethod;
    double m_amount;
    double m_refundedAmount;
    std::unique_ptr<TransactionState> m_state;
    std::chrono::system_clock::time_point m_timestamp;
    
    /**
     * @brief Generate a unique transaction ID
     * @return A unique transaction ID
     */
    static std::string generateTransactionId();
};

/**
 * @class TransactionFactory
 * @brief Factory for creating transactions (Factory Method Pattern)
 */
class TransactionFactory {
public:
    /**
     * @brief Create a new transaction
     * @param customer The customer making the payment
     * @param merchant The merchant receiving the payment
     * @param paymentMethod The payment method used
     * @param amount The transaction amount
     * @return A unique pointer to the created transaction
     */
    static std::unique_ptr<Transaction> createTransaction(
        const Customer& customer, const Merchant& merchant,
        std::unique_ptr<PaymentMethod> paymentMethod, double amount);
};

/**
 * @class PendingState
 * @brief Represents a pending transaction state
 */
class PendingState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

/**
 * @class ApprovedState
 * @brief Represents an approved transaction state
 */
class ApprovedState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

/**
 * @class DeclinedState
 * @brief Represents a declined transaction state
 */
class DeclinedState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

/**
 * @class FlaggedState
 * @brief Represents a flagged transaction state
 */
class FlaggedState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

/**
 * @class RefundedState
 * @brief Represents a refunded transaction state
 */
class RefundedState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

/**
 * @class PartiallyRefundedState
 * @brief Represents a partially refunded transaction state
 */
class PartiallyRefundedState : public TransactionState {
public:
    bool process(Transaction& transaction) override;
    bool refund(Transaction& transaction, double amount) override;
    TransactionStatus getStatus() const override;
    std::string toString() const override;
};

#endif // TRANSACTION_H
