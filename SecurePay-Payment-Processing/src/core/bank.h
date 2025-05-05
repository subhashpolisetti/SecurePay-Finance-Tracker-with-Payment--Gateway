#ifndef BANK_H
#define BANK_H

#include "transaction.h"
#include "fraudsystem.h"
#include "customer.h"

// Enum for authorization result
enum class AuthorizationResult {
    APPROVED,
    DECLINED,
    REVIEW_REQUIRED
};

// Singleton class for bank authorization
class Bank {
public:
   
    static Bank& getInstance();
    
  
    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;
    
    
    AuthorizationResult authorizeTransaction(const Transaction& transaction, 
                                            FraudRiskLevel fraudRiskLevel);
    
    
    static std::string resultToString(AuthorizationResult result);
    
private:
    
    Bank();
    
  
    bool hasSufficientFunds(const Transaction& transaction) const;
    bool isCardValid(const PaymentMethod& paymentMethod) const;
};

#endif
