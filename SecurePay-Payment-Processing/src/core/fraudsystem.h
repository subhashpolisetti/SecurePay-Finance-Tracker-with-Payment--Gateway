#ifndef FRAUDSYSTEM_H
#define FRAUDSYSTEM_H

#include <memory>
#include "transaction.h"

// Tunable thresholds for the rule-based fraud checks
namespace FraudRules {
    // Payments above this amount count as one suspicious factor
    constexpr double kLargeAmountThreshold = 1000.0;
    // Billing addresses containing any of these words count as one suspicious factor
    constexpr const char* kSuspiciousAddressKeywords[] = {"unknown", "test"};
    // Payment method types treated as higher risk
    constexpr const char* kHighRiskPaymentMethod = "Digital Wallet";
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
    
  
    static std::string riskLevelToString(FraudRiskLevel riskLevel);
    
private:
    
    FraudSystem();
    
  
    bool isAmountSuspicious(double amount) const;
    bool isLocationSuspicious(const std::string& billingAddress) const;
    bool isPaymentMethodSuspicious(const PaymentMethod& paymentMethod) const;
};

#endif 