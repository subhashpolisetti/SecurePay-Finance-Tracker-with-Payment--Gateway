#ifndef REFUNDMANAGER_H
#define REFUNDMANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "transaction.h"
#include "refund.h"

/**
 * @class RefundCommand
 * @brief Abstract base class for refund commands (Command Pattern)
 * 
 * This class follows the Command Pattern to encapsulate a refund request
 * as an object, allowing for parameterization of clients with different requests,
 * queuing of requests, and logging of operations.
 */
class RefundCommand {
public:
    virtual ~RefundCommand() = default;
    
    /**
     * @brief Execute the refund command
     * @return True if the refund was successful, false otherwise
     */
    virtual bool execute() = 0;
    
    /**
     * @brief Get the refund created by this command
     * @return Pointer to the refund, or nullptr if no refund was created
     */
    virtual Refund* getRefund() const = 0;
};

/**
 * @class FullRefundCommand
 * @brief Command for processing a full refund
 */
class FullRefundCommand : public RefundCommand {
public:
    /**
     * @brief Constructor for a full refund command
     * @param transaction The transaction to refund
     * @param reason The reason for the refund
     */
    FullRefundCommand(Transaction& transaction, const std::string& reason);
    
    /**
     * @brief Execute the full refund command
     * @return True if the refund was successful, false otherwise
     */
    bool execute() override;
    
    /**
     * @brief Get the refund created by this command
     * @return Pointer to the refund, or nullptr if no refund was created
     */
    Refund* getRefund() const override;
    
private:
    Transaction& m_transaction;
    std::string m_reason;
    std::unique_ptr<Refund> m_refund;
};

/**
 * @class PartialRefundCommand
 * @brief Command for processing a partial refund
 */
class PartialRefundCommand : public RefundCommand {
public:
    /**
     * @brief Constructor for a partial refund command
     * @param transaction The transaction to refund
     * @param amount The amount to refund
     * @param reason The reason for the refund
     */
    PartialRefundCommand(Transaction& transaction, double amount, const std::string& reason);
    
    /**
     * @brief Execute the partial refund command
     * @return True if the refund was successful, false otherwise
     */
    bool execute() override;
    
    /**
     * @brief Get the refund created by this command
     * @return Pointer to the refund, or nullptr if no refund was created
     */
    Refund* getRefund() const override;
    
private:
    Transaction& m_transaction;
    double m_amount;
    std::string m_reason;
    std::unique_ptr<Refund> m_refund;
};

/**
 * @class RefundManager
 * @brief Manages refund operations (Singleton Pattern)
 * 
 * This class follows the Singleton Pattern to ensure a single instance
 * and the Command Pattern to process refunds.
 */
class RefundManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static RefundManager& getInstance();
    
    /**
     * @brief Delete copy constructor
     */
    RefundManager(const RefundManager&) = delete;
    
    /**
     * @brief Delete assignment operator
     */
    RefundManager& operator=(const RefundManager&) = delete;
    
    /**
     * @brief Process a full refund
     * @param transaction The transaction to refund
     * @param reason The reason for the refund
     * @return True if the refund was successful, false otherwise
     */
    bool processFullRefund(Transaction& transaction, const std::string& reason);
    
    /**
     * @brief Process a partial refund
     * @param transaction The transaction to refund
     * @param amount The amount to refund
     * @param reason The reason for the refund
     * @return True if the refund was successful, false otherwise
     */
    bool processPartialRefund(Transaction& transaction, double amount, const std::string& reason);
    
    /**
     * @brief Get all refunds
     * @return Vector of refunds
     */
    const std::vector<std::unique_ptr<Refund>>& getRefunds() const;
    
    /**
     * @brief Get refunds for a specific transaction
     * @param transactionId The transaction ID
     * @return Vector of refunds for the transaction
     */
    std::vector<const Refund*> getRefundsForTransaction(const std::string& transactionId) const;
    
private:
    /**
     * @brief Private constructor for singleton
     */
    RefundManager();
    
    /**
     * @brief Process a refund command
     * @param command The refund command to process
     * @return True if the refund was successful, false otherwise
     */
    bool processRefundCommand(std::unique_ptr<RefundCommand> command);
    
    std::vector<std::unique_ptr<Refund>> m_refunds;
};

#endif // REFUNDMANAGER_H
