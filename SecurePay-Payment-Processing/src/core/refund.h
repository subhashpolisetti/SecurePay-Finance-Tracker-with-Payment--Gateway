#ifndef REFUND_H
#define REFUND_H

#include <string>
#include <memory>
#include <chrono>
#include "transaction.h"

/**
 * @class Refund
 * @brief Represents a refund for a transaction
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * refund data representation and basic operations.
 */
class Refund {
public:
    /**
     * @brief Constructor for a refund
     * @param transaction Reference to the original transaction
     * @param amount The amount to refund (can be partial)
     * @param reason The reason for the refund
     */
    Refund(const Transaction& transaction, double amount, const std::string& reason);
    
    /**
     * @brief Get the refund ID
     * @return The unique refund ID
     */
    std::string getRefundId() const;
    
    /**
     * @brief Get the original transaction
     * @return Reference to the original transaction
     */
    const Transaction& getTransaction() const;
    
    /**
     * @brief Get the refund amount
     * @return The amount refunded
     */
    double getAmount() const;
    
    /**
     * @brief Get the refund reason
     * @return The reason for the refund
     */
    std::string getReason() const;
    
    /**
     * @brief Get the timestamp of when the refund was created
     * @return The timestamp as a string
     */
    std::string getTimestamp() const;
    
private:
    std::string m_refundId;
    const Transaction& m_transaction;
    double m_amount;
    std::string m_reason;
    std::chrono::system_clock::time_point m_timestamp;
    
    /**
     * @brief Generate a unique refund ID
     * @return A unique refund ID
     */
    static std::string generateRefundId();
};

/**
 * @class RefundFactory
 * @brief Factory for creating refunds (Factory Method Pattern)
 */
class RefundFactory {
public:
    /**
     * @brief Create a new refund
     * @param transaction The original transaction
     * @param amount The amount to refund
     * @param reason The reason for the refund
     * @return A unique pointer to the created refund
     */
    static std::unique_ptr<Refund> createRefund(
        const Transaction& transaction, double amount, const std::string& reason);
};

#endif // REFUND_H
