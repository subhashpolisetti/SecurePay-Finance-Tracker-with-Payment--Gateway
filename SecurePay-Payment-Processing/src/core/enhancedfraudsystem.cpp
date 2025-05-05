#include "enhancedfraudsystem.h"
#include <iostream>
#include <algorithm>
#include <regex>

EnhancedFraudSystem& EnhancedFraudSystem::getInstance() {
    static EnhancedFraudSystem instance;
    return instance;
}

EnhancedFraudSystem::EnhancedFraudSystem()
    : m_highRiskThreshold(0.7f), m_mediumRiskThreshold(0.3f) {
    
    std::cout << "EnhancedFraudSystem initialized" << std::endl;
    
    // Initialize default rules with weights
    addRule("Large Amount", 0.4f, 
           [](const Transaction& t) { return isAmountSuspicious(t.getAmount()); },
           "Transaction amount exceeds $1000");
    
    addRule("Suspicious Address", 0.3f, 
           [](const Transaction& t) { return isLocationSuspicious(t.getCustomer().getBillingAddress()); },
           "Billing address contains suspicious keywords");
    
    addRule("Digital Wallet", 0.2f, 
           [](const Transaction& t) { return isPaymentMethodSuspicious(t.getPaymentMethod()); },
           "Payment method is a digital wallet");
    
    addRule("Odd Hour", 0.3f, 
           [](const Transaction& t) { return isTimeSuspicious(); },
           "Transaction occurred during suspicious hours (2 AM - 5 AM)");
}

FraudRiskLevel EnhancedFraudSystem::evaluateTransaction(const Transaction& transaction) {
    std::vector<std::string> triggeredRules;
    float score = getFraudScore(transaction, triggeredRules);
    
    FraudRiskLevel level;
    if (score >= m_highRiskThreshold) {
        level = FraudRiskLevel::HIGH;
    } else if (score >= m_mediumRiskThreshold) {
        level = FraudRiskLevel::MEDIUM;
    } else {
        level = FraudRiskLevel::LOW;
    }
    
    // Create fraud alert for medium and high risk transactions
    if (level != FraudRiskLevel::LOW) {
        createFraudAlert(transaction, level, triggeredRules, score);
    }
    
    return level;
}

float EnhancedFraudSystem::getFraudScore(const Transaction& transaction, std::vector<std::string>& triggeredRules) {
    float score = 0.0f;
    triggeredRules.clear();
    
    for (const auto& rule : m_rules) {
        if (rule.checkFunction(transaction)) {
            score += rule.weight;
            triggeredRules.push_back(rule.name);
        }
    }
    
    // Normalize score to be between 0 and 1
    return std::min(score, 1.0f);
}

void EnhancedFraudSystem::addRule(const std::string& name, float weight, 
                                 std::function<bool(const Transaction&)> checkFunction,
                                 const std::string& description) {
    // Check if rule already exists
    for (auto& rule : m_rules) {
        if (rule.name == name) {
            // Update existing rule
            rule.weight = weight;
            rule.checkFunction = checkFunction;
            rule.description = description;
            return;
        }
    }
    
    // Add new rule
    FraudRule rule;
    rule.name = name;
    rule.weight = weight;
    rule.checkFunction = checkFunction;
    rule.description = description;
    
    m_rules.push_back(rule);
}

bool EnhancedFraudSystem::removeRule(const std::string& name) {
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
                          [&name](const FraudRule& rule) { return rule.name == name; });
    
    if (it != m_rules.end()) {
        m_rules.erase(it);
        return true;
    }
    
    return false;
}

const std::vector<FraudRule>& EnhancedFraudSystem::getRules() const {
    return m_rules;
}

void EnhancedFraudSystem::setHighRiskThreshold(float threshold) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        m_highRiskThreshold = threshold;
    }
}

void EnhancedFraudSystem::setMediumRiskThreshold(float threshold) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        m_mediumRiskThreshold = threshold;
    }
}

float EnhancedFraudSystem::getHighRiskThreshold() const {
    return m_highRiskThreshold;
}

float EnhancedFraudSystem::getMediumRiskThreshold() const {
    return m_mediumRiskThreshold;
}

void EnhancedFraudSystem::createFraudAlert(const Transaction& transaction, 
                                          FraudRiskLevel level,
                                          const std::vector<std::string>& triggeredRules,
                                          float score) {
    std::string description = "Risk score: " + std::to_string(score) + ". Triggered rules: ";
    
    for (size_t i = 0; i < triggeredRules.size(); ++i) {
        if (i > 0) description += ", ";
        description += triggeredRules[i];
    }
    
    auto alert = FraudAlertFactory::createFraudAlert(transaction, level, description);
    
    // In a real implementation, this would save the alert to the database
    std::cout << "Created fraud alert: " << alert->getAlertId() << " with risk level " 
              << FraudSystem::riskLevelToString(level) << std::endl;
    std::cout << "Description: " << description << std::endl;
}

bool EnhancedFraudSystem::isAmountSuspicious(double amount) {
    // Transactions over $1000 are considered suspicious
    return amount > 1000.0;
}

bool EnhancedFraudSystem::isLocationSuspicious(const std::string& billingAddress) {
    // Check for suspicious keywords in the billing address
    std::regex pattern("(?i)(test|suspicious|fake|invalid|unknown)");
    return std::regex_search(billingAddress, pattern);
}

bool EnhancedFraudSystem::isPaymentMethodSuspicious(const PaymentMethod& paymentMethod) {
    // Digital wallets are considered slightly more suspicious
    return paymentMethod.getType() == "Digital Wallet";
}

bool EnhancedFraudSystem::isTimeSuspicious() {
    // Transactions between 2 AM and 5 AM are considered suspicious
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);
    int hour = local_tm.tm_hour;
    
    return hour >= 2 && hour <= 5;
}
