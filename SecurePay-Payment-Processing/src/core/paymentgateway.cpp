#include "paymentgateway.h"
#include <iostream>
#include <algorithm>

PaymentGateway::PaymentGateway() {
    std::cout << "PaymentGateway initialized" << std::endl;
}

void PaymentGateway::processTransaction(std::unique_ptr<Transaction> transaction) {
    std::cout << "Processing transaction " << transaction->getTransactionId() << std::endl;
    
    // First, ensure the transaction is processed through its state machine
    // This will move it from PENDING to APPROVED state
    transaction->process();
    
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
            std::cout << "Transaction approved" << std::endl;
            break;
        case AuthorizationResult::DECLINED:
            transaction->setState(std::make_unique<DeclinedState>());
            std::cout << "Transaction declined by bank" << std::endl;
            break;
        case AuthorizationResult::REVIEW_REQUIRED:
            transaction->setState(std::make_unique<FlaggedState>());
            std::cout << "Transaction flagged for review" << std::endl;
            break;
    }
    
    notifyObservers(*transaction);
    
    m_transactions.push_back(std::move(transaction));
}

void PaymentGateway::storeTransaction(std::unique_ptr<Transaction> transaction) {
    // Simply store the transaction without processing it
    // This is used when loading transactions from the database
    m_transactions.push_back(std::move(transaction));
}

std::string PaymentGateway::processTransactionWithIdempotencyKey(
    std::unique_ptr<Transaction> transaction,
    const std::string& idempotencyKey) {
    
    // Check if transaction with this idempotency key already exists
    Transaction* existingTransaction = findTransactionByIdempotencyKey(idempotencyKey);
    if (existingTransaction) {
        std::cout << "Transaction with idempotency key " << idempotencyKey << " already exists" << std::endl;
        return existingTransaction->getTransactionId();
    }
    
    // Set idempotency key on the transaction
    transaction->setIdempotencyKey(idempotencyKey);
    
    // Store the idempotency key mapping
    std::string transactionId = transaction->getTransactionId();
    m_idempotencyKeys[idempotencyKey] = transactionId;
    
    // Process the transaction
    processTransaction(std::move(transaction));
    
    return transactionId;
}

bool PaymentGateway::authorizeTransaction(std::unique_ptr<Transaction> transaction) {
    std::cout << "Authorizing transaction " << transaction->getTransactionId() << std::endl;
    
    encryptTransactionData(*transaction);
    
    FraudSystem& fraudSystem = FraudSystem::getInstance();
    FraudRiskLevel riskLevel = fraudSystem.evaluateTransaction(*transaction);
    
    std::cout << "Fraud risk level: " << FraudSystem::riskLevelToString(riskLevel) << std::endl;
    
    Bank& bank = Bank::getInstance();
    AuthorizationResult authResult = bank.authorizeTransaction(*transaction, riskLevel);
    
    std::cout << "Authorization result: " << Bank::resultToString(authResult) << std::endl;
    
    bool success = false;
    
    switch (authResult) {
        case AuthorizationResult::APPROVED:
            transaction->setState(std::make_unique<PreAuthorizedState>());
            std::cout << "Transaction pre-authorized" << std::endl;
            success = true;
            break;
        case AuthorizationResult::DECLINED:
            transaction->setState(std::make_unique<DeclinedState>());
            std::cout << "Transaction declined by bank" << std::endl;
            success = false;
            break;
        case AuthorizationResult::REVIEW_REQUIRED:
            transaction->setState(std::make_unique<FlaggedState>());
            std::cout << "Transaction flagged for review" << std::endl;
            success = false;
            break;
    }
    
    notifyObservers(*transaction);
    
    m_transactions.push_back(std::move(transaction));
    
    return success;
}

bool PaymentGateway::captureTransaction(const std::string& transactionId, double amount) {
    std::cout << "Capturing transaction " << transactionId << std::endl;
    
    Transaction* transaction = findTransaction(transactionId);
    if (!transaction) {
        std::cout << "Transaction not found" << std::endl;
        return false;
    }
    
    return transaction->capture(amount);
}

bool PaymentGateway::voidTransaction(const std::string& transactionId) {
    std::cout << "Voiding transaction " << transactionId << std::endl;
    
    Transaction* transaction = findTransaction(transactionId);
    if (!transaction) {
        std::cout << "Transaction not found" << std::endl;
        return false;
    }
    
    return transaction->voidTransaction();
}

const std::vector<std::unique_ptr<Transaction>>& PaymentGateway::getTransactions() const {
    return m_transactions;
}

Transaction* PaymentGateway::findTransaction(const std::string& transactionId) {
    for (auto& transaction : m_transactions) {
        if (transaction->getTransactionId() == transactionId) {
            return transaction.get();
        }
    }
    
    return nullptr;
}

Transaction* PaymentGateway::findTransactionByIdempotencyKey(const std::string& idempotencyKey) {
    auto it = m_idempotencyKeys.find(idempotencyKey);
    if (it != m_idempotencyKeys.end()) {
        return findTransaction(it->second);
    }
    
    return nullptr;
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
    
    // In a real implementation, this would use a secure encryption algorithm
    // to protect sensitive payment data like card numbers, CVVs, etc.
    // For example:
    // 1. Use AES-256 for encryption
    // 2. Use a secure key management system
    // 3. Ensure PCI DSS compliance
    
    // For credit card data, we would:
    // 1. Encrypt the full card number
    // 2. Store only the last 4 digits in plain text
    // 3. Never store the CVV after authorization
}
