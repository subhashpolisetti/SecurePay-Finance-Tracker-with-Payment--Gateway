#ifndef PAYMENTGATEWAY_H
#define PAYMENTGATEWAY_H

#include <vector>
#include <memory>
#include <functional>
#include "transaction.h"
#include "fraudsystem.h"
#include "bank.h"

// Observer interface for transaction updates (Observer pattern)
class TransactionObserver {
public:
    virtual ~TransactionObserver() = default;
    virtual void onTransactionUpdated(const Transaction& transaction) = 0;
};

// Class for processing payments
class PaymentGateway {
public:
    PaymentGateway();
    
    
    void processTransaction(std::unique_ptr<Transaction> transaction);
    

    const std::vector<std::unique_ptr<Transaction>>& getTransactions() const;
    
   
    void addObserver(TransactionObserver* observer);
    
    
    void removeObserver(TransactionObserver* observer);
    
private:
  
    std::vector<std::unique_ptr<Transaction>> m_transactions;
    
   
    std::vector<TransactionObserver*> m_observers;
    
   
    void notifyObservers(const Transaction& transaction);
    
    
    void encryptTransactionData(const Transaction& transaction);
};

#endif
