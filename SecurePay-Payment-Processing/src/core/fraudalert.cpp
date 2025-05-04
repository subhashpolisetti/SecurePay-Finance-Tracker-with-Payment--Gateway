#include "fraudalert.h"
#include <random>
#include <sstream>
#include <iomanip>

FraudAlert::FraudAlert(const Transaction& transaction, FraudRiskLevel riskLevel, const std::string& description)
    : m_transaction(transaction), 
      m_riskLevel(riskLevel),
      m_description(description),
      m_timestamp(std::chrono::system_clock::now()),
      m_reviewed(false) {
    m_alertId = generateAlertId();
}

std::string FraudAlert::getAlertId() const {
    return m_alertId;
}

const Transaction& FraudAlert::getTransaction() const {
    return m_transaction;
}

FraudRiskLevel FraudAlert::getRiskLevel() const {
    return m_riskLevel;
}

std::string FraudAlert::getDescription() const {
    return m_description;
}

std::string FraudAlert::getTimestamp() const {
    auto time = std::chrono::system_clock::to_time_t(m_timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool FraudAlert::isReviewed() const {
    return m_reviewed;
}

void FraudAlert::setReviewed(bool reviewed) {
    m_reviewed = reviewed;
}

std::string FraudAlert::generateAlertId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex_chars = "0123456789ABCDEF";
    
    std::stringstream ss;
    ss << "FA-";
    for (int i = 0; i < 8; ++i) {
        ss << hex_chars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; ++i) {
        ss << hex_chars[dis(gen)];
    }
    
    return ss.str();
}

std::unique_ptr<FraudAlert> FraudAlertFactory::createFraudAlert(
    const Transaction& transaction, FraudRiskLevel riskLevel, const std::string& description) {
    return std::make_unique<FraudAlert>(transaction, riskLevel, description);
}
