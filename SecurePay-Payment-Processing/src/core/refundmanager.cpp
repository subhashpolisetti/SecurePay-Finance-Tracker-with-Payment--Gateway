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
        // The refund to the customer's account will be handled by the RefundManager
        // which has access to the actual Customer objects
        m_refund = RefundFactory::createRefund(m_transaction, amount, m_reason);
        return true;
    }
    
    return false;
}

Refund* FullRefundCommand::getRefund() const {
    return m_refund.get();
}

// PartialRefundCommand implementation
PartialRefundCommand::PartialRefundCommand(Transaction& transaction, double amount, const std::string& reason)
    : m_transaction(transaction), m_amount(amount), m_reason(reason), m_refund(nullptr) {
}

bool PartialRefundCommand::execute() {
    if (m_transaction.refund(m_amount)) {
        // The refund to the customer's account will be handled by the RefundManager
        // which has access to the actual Customer objects
        m_refund = RefundFactory::createRefund(m_transaction, m_amount, m_reason);
        return true;
    }
    
    return false;
}

Refund* PartialRefundCommand::getRefund() const {
    return m_refund.get();
}

// RefundManager implementation
RefundManager& RefundManager::getInstance() {
    static RefundManager instance;
    return instance;
}

RefundManager::RefundManager() : m_customers(nullptr) {
    std::cout << "RefundManager initialized" << std::endl;
}

void RefundManager::setCustomers(std::vector<Customer>* customers) {
    m_customers = customers;
}

bool RefundManager::processFullRefund(Transaction& transaction, const std::string& reason) {
    return processRefundCommand(std::make_unique<FullRefundCommand>(transaction, reason));
}

bool RefundManager::processPartialRefund(Transaction& transaction, double amount, const std::string& reason) {
    return processRefundCommand(std::make_unique<PartialRefundCommand>(transaction, amount, reason));
}

bool RefundManager::processRefundCommand(std::unique_ptr<RefundCommand> command) {
    if (command->execute()) {
        Refund* refund = command->getRefund();
        if (refund) {
            std::cout << "Refund processed: " << refund->getRefundId() 
                      << " for transaction " << refund->getTransaction().getTransactionId()
                      << " amount: " << refund->getAmount() << std::endl;
            
            // Credit funds back to customer's account if we have access to the customers vector
            if (m_customers) {
                const std::string& customerName = refund->getTransaction().getCustomer().getName();
                const std::string& paymentMethodType = refund->getTransaction().getPaymentMethod().getType();
                double amount = refund->getAmount();
                
                // Find the customer by name
                for (auto& customer : *m_customers) {
                    if (customer.getName() == customerName) {
                        // Refund the amount to the customer's account
                        customer.refund(paymentMethodType, amount);
                        std::cout << "Refunded $" << amount << " to " << customerName 
                                  << "'s " << paymentMethodType << " account" << std::endl;
                        break;
                    }
                }
            }
            
            // Take ownership of the refund
            m_refunds.push_back(RefundFactory::createRefund(
                refund->getTransaction(), refund->getAmount(), refund->getReason()));
            
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
