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
    // Check if the payment method is a credit or debit card
    if (paymentMethod.getType() == "Credit Card" || paymentMethod.getType() == "Debit Card") {
        // Get the CVV from the payment method (this is the entered CVV)
        const std::string& enteredCVV = paymentMethod.getCVV();
        
        // In a real implementation, we would retrieve the stored CVV from a secure database
        // For this example, we'll simulate a stored CVV for demonstration purposes
        std::string storedCVV = "123"; // This would come from a secure database in a real system
        
        // Check if the entered CVV matches the stored CVV
        if (enteredCVV != storedCVV) {
            std::cout << "CVV validation failed: entered CVV does not match stored CVV" << std::endl;
            return false;
        }
        
        // For credit/debit cards, we would also check:
        // 1. Card number validity (Luhn algorithm)
        // 2. Expiration date validity
        
        std::cout << "Card validation successful" << std::endl;
        return true;
    }
    
    // For other payment methods (like digital wallets), different validation would apply
    return true;
}

bool Bank::hasSufficientFunds(const Transaction& transaction) const {
    const Customer& customer = transaction.getCustomer();
    const std::string& paymentMethodType = transaction.getPaymentMethod().getType();
    double balance = customer.getBalance(paymentMethodType);
    double amount = transaction.getAmount();
    
    std::cout << "Checking funds for " << paymentMethodType << ": Balance $" << balance 
              << ", Transaction amount $" << amount << std::endl;
    
    return balance >= amount;
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
