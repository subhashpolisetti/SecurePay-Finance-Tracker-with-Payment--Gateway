#include "fraudsystem.h"
#include <algorithm>
#include <iostream>

FraudSystem& FraudSystem::getInstance() {
    static FraudSystem instance;
    return instance;
}

FraudSystem::FraudSystem() {
    std::cout << "FraudSystem initialized" << std::endl;
}

FraudRiskLevel FraudSystem::evaluateTransaction(const Transaction& transaction) {
    std::cout << "Evaluating transaction " << transaction.getTransactionId() 
              << " for fraud risk" << std::endl;
    
    int suspiciousFactors = 0;
    
    if (isAmountSuspicious(transaction.getAmount())) {
        suspiciousFactors++;
    }
    
    if (isLocationSuspicious(transaction.getCustomer().getBillingAddress())) {
        suspiciousFactors++;
    }
    
    if (isPaymentMethodSuspicious(transaction.getPaymentMethod())) {
        suspiciousFactors++;
    }
    
    if (suspiciousFactors == 0) {
        return FraudRiskLevel::LOW;
    } else if (suspiciousFactors == 1) {
        return FraudRiskLevel::MEDIUM;
    } else {
        return FraudRiskLevel::HIGH;
    }
}

bool FraudSystem::isAmountSuspicious(double amount) const {
    return amount > FraudRules::kLargeAmountThreshold;
}

bool FraudSystem::isLocationSuspicious(const std::string& billingAddress) const {
    std::string lowerAddress = billingAddress;
    std::transform(lowerAddress.begin(), lowerAddress.end(), lowerAddress.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    for (const char* keyword : FraudRules::kSuspiciousAddressKeywords) {
        if (lowerAddress.find(keyword) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool FraudSystem::isPaymentMethodSuspicious(const PaymentMethod& paymentMethod) const {
    return paymentMethod.getType() == FraudRules::kHighRiskPaymentMethod;
}

std::string FraudSystem::riskLevelToString(FraudRiskLevel riskLevel) {
    switch (riskLevel) {
        case FraudRiskLevel::LOW:
            return "Low";
        case FraudRiskLevel::MEDIUM:
            return "Medium";
        case FraudRiskLevel::HIGH:
            return "High";
        default:
            return "Unknown";
    }
}
