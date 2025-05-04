#ifndef FRAUDSYSTEM_H
#define FRAUDSYSTEM_H

#include <memory>
#include "transaction.h"

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