#include "bank.h"
#include <iostream>

Bank& Bank::getInstance() {
    static Bank instance;
    return instance;
}

Bank::Bank() {
    std::cout << "Bank initialized" << std::endl;
}

AuthorizationResult Bank::authorizeTransaction(const Transaction& transaction, 
                                              FraudRiskLevel fraudRiskLevel) {
    std::cout << "Authorizing transaction " << transaction.getTransactionId() << std::endl;
    
    if (!isCardValid(transaction.getPaymentMethod())) {
        std::cout << "Card validation failed" << std::endl;
        return AuthorizationResult::DECLINED;
    }
    
    if (!hasSufficientFunds(transaction)) {
        std::cout << "Insufficient funds" << std::endl;
        return AuthorizationResult::DECLINED;
    }
    
    if (fraudRiskLevel == FraudRiskLevel::HIGH) {
        std::cout << "High fraud risk, review required" << std::endl;
        return AuthorizationResult::REVIEW_REQUIRED;
    } else if (fraudRiskLevel == FraudRiskLevel::MEDIUM) {
        std::cout << "Medium fraud risk, but approved" << std::endl;
        return AuthorizationResult::APPROVED;
    }
    
    std::cout << "Transaction approved" << std::endl;
    return AuthorizationResult::APPROVED;
}

bool Bank::isCardValid(const PaymentMethod& paymentMethod) const {
    return true;
}

bool Bank::hasSufficientFunds(const Transaction& transaction) const {
    return transaction.getAmount() < 5000.0;
}

std::string Bank::resultToString(AuthorizationResult result) {
    switch (result) {
        case AuthorizationResult::APPROVED:
            return "Approved";
        case AuthorizationResult::DECLINED:
            return "Declined";
        case AuthorizationResult::REVIEW_REQUIRED:
            return "Flagged for Review";
        default:
            return "Unknown";
    }
}
