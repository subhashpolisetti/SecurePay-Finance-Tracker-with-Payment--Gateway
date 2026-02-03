#include "refundmanager.h"
#include <iostream>
#include <algorithm>

// FullRefundCommand implementation
FullRefundCommand::FullRefundCommand(Transaction& transaction, const std::string& reason)
    : m_transaction(transaction), m_reason(reason), m_refund(nullptr) {
}

bool FullRefundCommand::execute() {
    double amount = m_transaction.getRemainingAmount();
    
    if (m_transaction.refund(amount)) {
        m_refund = RefundFactory::createRefund(m_transaction, amount, m_reason);
        return true;
    }
    
    return false;
}

Refund* FullRefundCommand::getRefund() const {
    return m_refund.get();
}

std::unique_ptr<Refund> FullRefundCommand::releaseRefund() {
    return std::move(m_refund);
}

// PartialRefundCommand implementation
PartialRefundCommand::PartialRefundCommand(Transaction& transaction, double amount, const std::string& reason)
    : m_transaction(transaction), m_amount(amount), m_reason(reason), m_refund(nullptr) {
}

bool PartialRefundCommand::execute() {
    if (m_transaction.refund(m_amount)) {
        m_refund = RefundFactory::createRefund(m_transaction, m_amount, m_reason);
        return true;
    }
    
    return false;
}

Refund* PartialRefundCommand::getRefund() const {
    return m_refund.get();
}

std::unique_ptr<Refund> PartialRefundCommand::releaseRefund() {
    return std::move(m_refund);
}

// RefundManager implementation
RefundManager& RefundManager::getInstance() {
    static RefundManager instance;
    return instance;
}

RefundManager::RefundManager() {
    std::cout << "RefundManager initialized" << std::endl;
}

bool RefundManager::processFullRefund(Transaction& transaction, const std::string& reason) {
    return processRefundCommand(std::make_unique<FullRefundCommand>(transaction, reason));
}

bool RefundManager::processPartialRefund(Transaction& transaction, double amount, const std::string& reason) {
    return processRefundCommand(std::make_unique<PartialRefundCommand>(transaction, amount, reason));
}

bool RefundManager::processRefundCommand(std::unique_ptr<RefundCommand> command) {
    if (command->execute()) {
        std::unique_ptr<Refund> refund = command->releaseRefund();
        if (refund) {
            std::cout << "Refund processed: " << refund->getRefundId() 
                      << " for transaction " << refund->getTransaction().getTransactionId()
                      << " amount: " << refund->getAmount() << std::endl;
            
            // Keep the refund the command created so its ID and timestamp stay the same
            m_refunds.push_back(std::move(refund));
            
            return true;
        }
    }
    
    return false;
}

const std::vector<std::unique_ptr<Refund>>& RefundManager::getRefunds() const {
    return m_refunds;
}

std::vector<const Refund*> RefundManager::getRefundsForTransaction(const std::string& transactionId) const {
    std::vector<const Refund*> result;
    
    for (const auto& refund : m_refunds) {
        if (refund->getTransaction().getTransactionId() == transactionId) {
            result.push_back(refund.get());
        }
    }
    
    return result;
}
