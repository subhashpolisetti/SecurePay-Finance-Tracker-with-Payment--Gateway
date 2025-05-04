#include "refund.h"
#include <random>
#include <sstream>
#include <iomanip>

Refund::Refund(const Transaction& transaction, double amount, const std::string& reason)
    : m_transaction(transaction), 
      m_amount(amount),
      m_reason(reason),
      m_timestamp(std::chrono::system_clock::now()) {
    m_refundId = generateRefundId();
}

std::string Refund::getRefundId() const {
    return m_refundId;
}

const Transaction& Refund::getTransaction() const {
    return m_transaction;
}

double Refund::getAmount() const {
    return m_amount;
}

std::string Refund::getReason() const {
    return m_reason;
}

std::string Refund::getTimestamp() const {
    auto time = std::chrono::system_clock::to_time_t(m_timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Refund::generateRefundId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex_chars = "0123456789ABCDEF";
    
    std::stringstream ss;
    ss << "RF-";
    for (int i = 0; i < 8; ++i) {
        ss << hex_chars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; ++i) {
        ss << hex_chars[dis(gen)];
    }
    
    return ss.str();
}

std::unique_ptr<Refund> RefundFactory::createRefund(
    const Transaction& transaction, double amount, const std::string& reason) {
    return std::make_unique<Refund>(transaction, amount, reason);
}
