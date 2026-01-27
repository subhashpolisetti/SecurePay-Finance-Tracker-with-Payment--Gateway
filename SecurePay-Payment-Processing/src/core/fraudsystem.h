#ifndef FRAUDSYSTEM_H
#define FRAUDSYSTEM_H

#include <chrono>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "transaction.h"

// Tunable thresholds for the rule-based fraud checks
namespace FraudRules {
    // Payments above this amount count as one suspicious factor
    constexpr double kLargeAmountThreshold = 1000.0;
    // Billing addresses containing any of these words count as one suspicious factor
    constexpr const char* kSuspiciousAddressKeywords[] = {"unknown", "test"};
    // Payment method types treated as higher risk
    constexpr const char* kHighRiskPaymentMethod = "Digital Wallet";
    // More than this many payments by one customer inside the window counts as one suspicious factor
    constexpr int kVelocityMaxTransactions = 3;
    constexpr std::chrono::seconds kVelocityWindow{60};
}

// Enum for fraud risk level
enum class FraudRiskLevel {
    LOW,
    MEDIUM,
    HIGH
};

// Singleton class for fraud detection
class FraudSystem {
public:
    
    static FraudSystem& getInstance();
    

    FraudSystem(const FraudSystem&) = delete;
    FraudSystem& operator=(const FraudSystem&) = delete;
    

    FraudRiskLevel evaluateTransaction(const Transaction& transaction);
    
    // Same as above with an explicit clock reading, so velocity checks can be tested deterministically
    FraudRiskLevel evaluateTransaction(const Transaction& transaction,
                                       std::chrono::system_clock::time_point now);
    
  
    static std::string riskLevelToString(FraudRiskLevel riskLevel);
    
private:
    
    FraudSystem();
    
  
    bool isAmountSuspicious(double amount) const;
    bool isLocationSuspicious(const std::string& billingAddress) const;
    bool isPaymentMethodSuspicious(const PaymentMethod& paymentMethod) const;
    
    // Records this payment and reports whether the customer is paying unusually often
    bool isVelocitySuspicious(const Customer& customer, std::chrono::system_clock::time_point now);
    
    std::mutex m_mutex;
    std::map<std::string, std::deque<std::chrono::system_clock::time_point>> m_recentPayments;
};

#endif 