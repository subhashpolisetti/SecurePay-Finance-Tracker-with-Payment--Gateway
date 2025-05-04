#include "transactiondecorator.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

// TransactionDecorator implementation
TransactionDecorator::TransactionDecorator(std::unique_ptr<Transaction> transaction)
    : Transaction(transaction->getCustomer(), transaction->getMerchant(), 
                 std::unique_ptr<PaymentMethod>(transaction->getPaymentMethod().clone()), 
                 transaction->getAmount()),
      m_transaction(std::move(transaction)) {
}

std::string TransactionDecorator::getTransactionId() const {
    return m_transaction->getTransactionId();
}

const Customer& TransactionDecorator::getCustomer() const {
    return m_transaction->getCustomer();
}

const Merchant& TransactionDecorator::getMerchant() const {
    return m_transaction->getMerchant();
}

const PaymentMethod& TransactionDecorator::getPaymentMethod() const {
    return m_transaction->getPaymentMethod();
}

double TransactionDecorator::getAmount() const {
    return m_transaction->getAmount();
}

double TransactionDecorator::getRemainingAmount() const {
    return m_transaction->getRemainingAmount();
}

double TransactionDecorator::getRefundedAmount() const {
    return m_transaction->getRefundedAmount();
}

TransactionStatus TransactionDecorator::getStatus() const {
    return m_transaction->getStatus();
}

std::string TransactionDecorator::getTimestamp() const {
    return m_transaction->getTimestamp();
}

bool TransactionDecorator::process() {
    return m_transaction->process();
}

bool TransactionDecorator::refund(double amount) {
    return m_transaction->refund(amount);
}

void TransactionDecorator::setState(std::unique_ptr<TransactionState> state) {
    m_transaction->setState(std::move(state));
}

void TransactionDecorator::addRefundedAmount(double amount) {
    m_transaction->addRefundedAmount(amount);
}

// EncryptedTransaction implementation
EncryptedTransaction::EncryptedTransaction(std::unique_ptr<Transaction> transaction)
    : TransactionDecorator(std::move(transaction)) {
}

bool EncryptedTransaction::process() {
    std::cout << "Encrypting transaction data before processing..." << std::endl;
    encryptData();
    
    bool result = m_transaction->process();
    
    std::cout << "Decrypting transaction data after processing..." << std::endl;
    decryptData();
    
    return result;
}

void EncryptedTransaction::encryptData() {
    // In a real implementation, this would use a proper encryption algorithm
    // For this prototype, we'll just simulate encryption
    std::cout << "Transaction data encrypted using AES-256" << std::endl;
}

void EncryptedTransaction::decryptData() {
    // In a real implementation, this would use a proper decryption algorithm
    // For this prototype, we'll just simulate decryption
    std::cout << "Transaction data decrypted" << std::endl;
}

// LoggedTransaction implementation
LoggedTransaction::LoggedTransaction(std::unique_ptr<Transaction> transaction, const std::string& logFile)
    : TransactionDecorator(std::move(transaction)), m_logFile(logFile) {
}

bool LoggedTransaction::process() {
    logMessage("Processing transaction " + m_transaction->getTransactionId());
    
    bool result = m_transaction->process();
    
    logMessage("Transaction " + m_transaction->getTransactionId() + 
               " processed with result: " + (result ? "success" : "failure"));
    
    return result;
}

bool LoggedTransaction::refund(double amount) {
    logMessage("Refunding " + std::to_string(amount) + " from transaction " + 
               m_transaction->getTransactionId());
    
    bool result = m_transaction->refund(amount);
    
    logMessage("Refund for transaction " + m_transaction->getTransactionId() + 
               " processed with result: " + (result ? "success" : "failure"));
    
    return result;
}

void LoggedTransaction::logMessage(const std::string& message) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // In a real implementation, this would write to a log file
    // For this prototype, we'll just print to the console
    std::cout << "[" << timestamp.str() << "] " << message << std::endl;
    
    // Optionally write to a log file if specified
    if (!m_logFile.empty()) {
        std::ofstream logStream(m_logFile, std::ios::app);
        if (logStream.is_open()) {
            logStream << "[" << timestamp.str() << "] " << message << std::endl;
            logStream.close();
        }
    }
}

// TransactionDecoratorFactory implementation
std::unique_ptr<Transaction> TransactionDecoratorFactory::createEncryptedTransaction(
    std::unique_ptr<Transaction> transaction) {
    return std::make_unique<EncryptedTransaction>(std::move(transaction));
}

std::unique_ptr<Transaction> TransactionDecoratorFactory::createLoggedTransaction(
    std::unique_ptr<Transaction> transaction, const std::string& logFile) {
    return std::make_unique<LoggedTransaction>(std::move(transaction), logFile);
}

std::unique_ptr<Transaction> TransactionDecoratorFactory::createEncryptedLoggedTransaction(
    std::unique_ptr<Transaction> transaction, const std::string& logFile) {
    // First encrypt the transaction, then add logging
    auto encryptedTransaction = std::make_unique<EncryptedTransaction>(std::move(transaction));
    return std::make_unique<LoggedTransaction>(std::move(encryptedTransaction), logFile);
}
