#include "transaction.h"
#include "merchant.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>

// Transaction implementation
Transaction::Transaction(const Customer& customer, const Merchant& merchant,
                         std::unique_ptr<PaymentMethod> paymentMethod, double amount)
    : m_customer(customer),
      m_merchant(merchant),
      m_paymentMethod(std::move(paymentMethod)),
      m_amount(amount),
      m_refundedAmount(0.0),
      m_timestamp(std::chrono::system_clock::now()) {
    m_transactionId = generateTransactionId();
    m_state = std::make_unique<PendingState>();
}

std::string Transaction::getTransactionId() const {
    return m_transactionId;
}

const Customer& Transaction::getCustomer() const {
    return m_customer;
}

const Merchant& Transaction::getMerchant() const {
    return m_merchant;
}

const PaymentMethod& Transaction::getPaymentMethod() const {
    return *m_paymentMethod;
}

double Transaction::getAmount() const {
    return m_amount;
}

double Transaction::getRemainingAmount() const {
    return m_amount - m_refundedAmount;
}

double Transaction::getRefundedAmount() const {
    return m_refundedAmount;
}

TransactionStatus Transaction::getStatus() const {
    return m_state->getStatus();
}

std::string Transaction::getTimestamp() const {
    auto time = std::chrono::system_clock::to_time_t(m_timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Transaction::process() {
    return m_state->process(*this);
}

bool Transaction::refund(double amount) {
    return m_state->refund(*this, amount);
}

void Transaction::setState(std::unique_ptr<TransactionState> state) {
    m_state = std::move(state);
}

void Transaction::addRefundedAmount(double amount) {
    m_refundedAmount += amount;
}

bool Transaction::deductFundsFromCustomer(Customer& customer) {
    return customer.deduct(m_paymentMethod->getType(), m_amount);
}

std::string Transaction::statusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::PENDING:
            return "Pending";
        case TransactionStatus::APPROVED:
            return "Approved";
        case TransactionStatus::DECLINED:
            return "Declined";
        case TransactionStatus::FLAGGED_FOR_REVIEW:
            return "Flagged for Review";
        case TransactionStatus::REFUNDED:
            return "Refunded";
        case TransactionStatus::PARTIALLY_REFUNDED:
            return "Partially Refunded";
        default:
            return "Unknown";
    }
}

std::string Transaction::generateTransactionId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex_chars = "0123456789ABCDEF";
    
    std::stringstream ss;
    ss << "TX-";
    for (int i = 0; i < 8; ++i) {
        ss << hex_chars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; ++i) {
        ss << hex_chars[dis(gen)];
    }
    
    return ss.str();
}

// TransactionFactory implementation
std::unique_ptr<Transaction> TransactionFactory::createTransaction(
    const Customer& customer, const Merchant& merchant,
    std::unique_ptr<PaymentMethod> paymentMethod, double amount) {
    return std::make_unique<Transaction>(customer, merchant, std::move(paymentMethod), amount);
}

// PendingState implementation
bool PendingState::process(Transaction& transaction) {
    std::cout << "Processing transaction " << transaction.getTransactionId() << " from pending state" << std::endl;
    
    // In a real system, we would process the payment here
    // For now, we'll just transition to the approved state
    transaction.setState(std::make_unique<ApprovedState>());
    return true;
}

bool PendingState::refund(Transaction& transaction, double amount) {
    std::cout << "Cannot refund a pending transaction" << std::endl;
    return false;
}

TransactionStatus PendingState::getStatus() const {
    return TransactionStatus::PENDING;
}

std::string PendingState::toString() const {
    return "Pending";
}

// ApprovedState implementation
bool ApprovedState::process(Transaction& transaction) {
    std::cout << "Transaction " << transaction.getTransactionId() << " is already approved" << std::endl;
    return false;
}

bool ApprovedState::refund(Transaction& transaction, double amount) {
    std::cout << "Refunding " << amount << " from transaction " << transaction.getTransactionId() << std::endl;
    
    if (amount <= 0 || amount > transaction.getRemainingAmount()) {
        std::cout << "Invalid refund amount" << std::endl;
        return false;
    }
    
    transaction.addRefundedAmount(amount);
    
    if (transaction.getRemainingAmount() <= 0.001) { // Using a small epsilon for floating-point comparison
        transaction.setState(std::make_unique<RefundedState>());
    } else {
        transaction.setState(std::make_unique<PartiallyRefundedState>());
    }
    
    return true;
}

TransactionStatus ApprovedState::getStatus() const {
    return TransactionStatus::APPROVED;
}

std::string ApprovedState::toString() const {
    return "Approved";
}

// DeclinedState implementation
bool DeclinedState::process(Transaction& transaction) {
    std::cout << "Cannot process a declined transaction" << std::endl;
    return false;
}

bool DeclinedState::refund(Transaction& transaction, double amount) {
    std::cout << "Cannot refund a declined transaction" << std::endl;
    return false;
}

TransactionStatus DeclinedState::getStatus() const {
    return TransactionStatus::DECLINED;
}

std::string DeclinedState::toString() const {
    return "Declined";
}

// FlaggedState implementation
bool FlaggedState::process(Transaction& transaction) {
    std::cout << "Transaction " << transaction.getTransactionId() << " requires manual review" << std::endl;
    return false;
}

bool FlaggedState::refund(Transaction& transaction, double amount) {
    std::cout << "Cannot refund a flagged transaction" << std::endl;
    return false;
}

TransactionStatus FlaggedState::getStatus() const {
    return TransactionStatus::FLAGGED_FOR_REVIEW;
}

std::string FlaggedState::toString() const {
    return "Flagged for Review";
}

// RefundedState implementation
bool RefundedState::process(Transaction& transaction) {
    std::cout << "Cannot process a refunded transaction" << std::endl;
    return false;
}

bool RefundedState::refund(Transaction& transaction, double amount) {
    std::cout << "Transaction " << transaction.getTransactionId() << " is already fully refunded" << std::endl;
    return false;
}

TransactionStatus RefundedState::getStatus() const {
    return TransactionStatus::REFUNDED;
}

std::string RefundedState::toString() const {
    return "Refunded";
}

// PartiallyRefundedState implementation
bool PartiallyRefundedState::process(Transaction& transaction) {
    std::cout << "Cannot process a partially refunded transaction" << std::endl;
    return false;
}

bool PartiallyRefundedState::refund(Transaction& transaction, double amount) {
    std::cout << "Refunding additional " << amount << " from transaction " << transaction.getTransactionId() << std::endl;
    
    if (amount <= 0 || amount > transaction.getRemainingAmount()) {
        std::cout << "Invalid refund amount" << std::endl;
        return false;
    }
    
    transaction.addRefundedAmount(amount);
    
    if (transaction.getRemainingAmount() <= 0.001) { // Using a small epsilon for floating-point comparison
        transaction.setState(std::make_unique<RefundedState>());
    }
    
    return true;
}

TransactionStatus PartiallyRefundedState::getStatus() const {
    return TransactionStatus::PARTIALLY_REFUNDED;
}

std::string PartiallyRefundedState::toString() const {
    return "Partially Refunded";
}
