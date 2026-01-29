#include "paymentgateway.h"
#include <iostream>
#include <algorithm>

PaymentGateway::PaymentGateway() {
    std::cout << "PaymentGateway initialized" << std::endl;
}

void PaymentGateway::processTransaction(std::unique_ptr<Transaction> transaction) {
    std::cout << "Processing transaction " << transaction->getTransactionId() << std::endl;
    
    encryptTransactionData(*transaction);
    
    FraudSystem& fraudSystem = FraudSystem::getInstance();
    FraudRiskLevel riskLevel = fraudSystem.evaluateTransaction(*transaction);
    
    std::cout << "Fraud risk level: " << FraudSystem::riskLevelToString(riskLevel) << std::endl;
    
    Bank& bank = Bank::getInstance();
    AuthorizationResult authResult = bank.authorizeTransaction(*transaction, riskLevel);
    
    std::cout << "Authorization result: " << Bank::resultToString(authResult) << std::endl;
    
    switch (authResult) {
        case AuthorizationResult::APPROVED:
            transaction->setState(std::make_unique<ApprovedState>());
            break;
        case AuthorizationResult::DECLINED:
            transaction->setState(std::make_unique<DeclinedState>());
            break;
        case AuthorizationResult::REVIEW_REQUIRED:
            transaction->setState(std::make_unique<FlaggedState>());
            break;
    }
    
    recordFraudAlert(*transaction, riskLevel, authResult);
    
    notifyObservers(*transaction);
    
    m_transactions.push_back(std::move(transaction));
}

void PaymentGateway::recordFraudAlert(const Transaction& transaction, FraudRiskLevel riskLevel,
                                      AuthorizationResult authResult) {
    if (riskLevel == FraudRiskLevel::LOW) {
        return;
    }
    
    // The alert refers to the heap-allocated transaction, which keeps its address once stored
    std::string description = FraudSystem::riskLevelToString(riskLevel) + " fraud risk; bank result: " +
                              Bank::resultToString(authResult);
    m_fraudAlerts.push_back(FraudAlertFactory::createFraudAlert(transaction, riskLevel, description));
}

const std::vector<std::unique_ptr<Transaction>>& PaymentGateway::getTransactions() const {
    return m_transactions;
}

const std::vector<std::unique_ptr<FraudAlert>>& PaymentGateway::getFraudAlerts() const {
    return m_fraudAlerts;
}

void PaymentGateway::addObserver(TransactionObserver* observer) {
    if (observer) {
        m_observers.push_back(observer);
    }
}

void PaymentGateway::removeObserver(TransactionObserver* observer) {
    auto it = std::find(m_observers.begin(), m_observers.end(), observer);
    if (it != m_observers.end()) {
        m_observers.erase(it);
    }
}

void PaymentGateway::notifyObservers(const Transaction& transaction) {
    for (auto observer : m_observers) {
        observer->onTransactionUpdated(transaction);
    }
}

void PaymentGateway::encryptTransactionData(const Transaction& transaction) {
  
    // For the prototype, we are just simulating it without actual encryption
    std::cout << "Encrypting transaction data for " << transaction.getTransactionId() << std::endl;
}
