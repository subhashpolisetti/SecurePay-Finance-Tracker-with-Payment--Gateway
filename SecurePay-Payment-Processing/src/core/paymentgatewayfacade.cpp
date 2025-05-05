#include "paymentgatewayfacade.h"
#include <iostream>
#include <algorithm>

PaymentGatewayFacade::PaymentGatewayFacade(
    PaymentGateway& paymentGateway, Bank& bank, FraudSystem& fraudSystem)
    : m_paymentGateway(paymentGateway), m_bank(bank), m_fraudSystem(fraudSystem) {
}

Result<std::string> PaymentGatewayFacade::processPayment(
    const Customer& customer,
    const Merchant& merchant,
    const std::string& paymentMethodType,
    const std::vector<std::string>& paymentDetails,
    double amount) {
    
    // Validate input
    if (amount <= 0) {
        return Result<std::string>::failure(
            "INVALID_AMOUNT", "Payment amount must be greater than zero");
    }
    
    // Create payment method
    auto paymentMethod = createPaymentMethod(paymentMethodType, paymentDetails);
    if (!paymentMethod) {
        return Result<std::string>::failure(
            "INVALID_PAYMENT_METHOD", "Failed to create payment method");
    }
    
    try {
        // Create transaction
        auto transaction = TransactionFactory::createTransaction(
            customer, merchant, std::move(paymentMethod), amount);
        
        // Process transaction
        std::string transactionId = transaction->getTransactionId();
        m_paymentGateway.processTransaction(std::move(transaction));
        
        return Result<std::string>::success(transactionId);
    } catch (const std::exception& e) {
        return Result<std::string>::failure(
            "PROCESSING_ERROR", std::string("Error processing payment: ") + e.what());
    }
}

Result<std::string> PaymentGatewayFacade::processPaymentWithIdempotencyKey(
    const Customer& customer,
    const Merchant& merchant,
    const std::string& paymentMethodType,
    const std::vector<std::string>& paymentDetails,
    double amount,
    const std::string& idempotencyKey) {
    
    // Validate input
    if (amount <= 0) {
        return Result<std::string>::failure(
            "INVALID_AMOUNT", "Payment amount must be greater than zero");
    }
    
    if (idempotencyKey.empty()) {
        return Result<std::string>::failure(
            "INVALID_IDEMPOTENCY_KEY", "Idempotency key cannot be empty");
    }
    
    // Create payment method
    auto paymentMethod = createPaymentMethod(paymentMethodType, paymentDetails);
    if (!paymentMethod) {
        return Result<std::string>::failure(
            "INVALID_PAYMENT_METHOD", "Failed to create payment method");
    }
    
    try {
        // Create transaction
        auto transaction = TransactionFactory::createTransactionWithIdempotencyKey(
            customer, merchant, std::move(paymentMethod), amount, idempotencyKey);
        
        // Process transaction with idempotency key
        std::string transactionId = m_paymentGateway.processTransactionWithIdempotencyKey(
            std::move(transaction), idempotencyKey);
        
        return Result<std::string>::success(transactionId);
    } catch (const std::exception& e) {
        return Result<std::string>::failure(
            "PROCESSING_ERROR", std::string("Error processing payment: ") + e.what());
    }
}

Result<std::string> PaymentGatewayFacade::authorizePayment(
    const Customer& customer,
    const Merchant& merchant,
    const std::string& paymentMethodType,
    const std::vector<std::string>& paymentDetails,
    double amount) {
    
    // Validate input
    if (amount <= 0) {
        return Result<std::string>::failure(
            "INVALID_AMOUNT", "Payment amount must be greater than zero");
    }
    
    // Create payment method
    auto paymentMethod = createPaymentMethod(paymentMethodType, paymentDetails);
    if (!paymentMethod) {
        return Result<std::string>::failure(
            "INVALID_PAYMENT_METHOD", "Failed to create payment method");
    }
    
    try {
        // Create transaction
        auto transaction = TransactionFactory::createTransaction(
            customer, merchant, std::move(paymentMethod), amount);
        
        // Authorize transaction
        std::string transactionId = transaction->getTransactionId();
        bool success = m_paymentGateway.authorizeTransaction(std::move(transaction));
        
        if (success) {
            return Result<std::string>::success(transactionId);
        } else {
            return Result<std::string>::failure(
                "AUTHORIZATION_FAILED", "Failed to authorize payment");
        }
    } catch (const std::exception& e) {
        return Result<std::string>::failure(
            "PROCESSING_ERROR", std::string("Error authorizing payment: ") + e.what());
    }
}

Result<void> PaymentGatewayFacade::capturePayment(const std::string& transactionId, double amount) {
    if (transactionId.empty()) {
        return Result<void>::failure(
            "INVALID_TRANSACTION_ID", "Transaction ID cannot be empty");
    }
    
    try {
        bool success = m_paymentGateway.captureTransaction(transactionId, amount);
        
        if (success) {
            return Result<void>::success();
        } else {
            return Result<void>::failure(
                "CAPTURE_FAILED", "Failed to capture payment");
        }
    } catch (const std::exception& e) {
        return Result<void>::failure(
            "PROCESSING_ERROR", std::string("Error capturing payment: ") + e.what());
    }
}

Result<void> PaymentGatewayFacade::voidPayment(const std::string& transactionId) {
    if (transactionId.empty()) {
        return Result<void>::failure(
            "INVALID_TRANSACTION_ID", "Transaction ID cannot be empty");
    }
    
    try {
        bool success = m_paymentGateway.voidTransaction(transactionId);
        
        if (success) {
            return Result<void>::success();
        } else {
            return Result<void>::failure(
                "VOID_FAILED", "Failed to void payment");
        }
    } catch (const std::exception& e) {
        return Result<void>::failure(
            "PROCESSING_ERROR", std::string("Error voiding payment: ") + e.what());
    }
}

const Transaction* PaymentGatewayFacade::getTransaction(const std::string& transactionId) const {
    for (const auto& transaction : m_paymentGateway.getTransactions()) {
        if (transaction->getTransactionId() == transactionId) {
            return transaction.get();
        }
    }
    
    return nullptr;
}

std::vector<const Transaction*> PaymentGatewayFacade::getAllTransactions() const {
    std::vector<const Transaction*> result;
    
    for (const auto& transaction : m_paymentGateway.getTransactions()) {
        result.push_back(transaction.get());
    }
    
    return result;
}

std::vector<const Transaction*> PaymentGatewayFacade::getTransactionsForCustomer(const std::string& customerId) const {
    std::vector<const Transaction*> result;
    
    for (const auto& transaction : m_paymentGateway.getTransactions()) {
        if (transaction->getCustomer().getName() == customerId) {
            result.push_back(transaction.get());
        }
    }
    
    return result;
}

std::vector<const Transaction*> PaymentGatewayFacade::getTransactionsForMerchant(const std::string& merchantId) const {
    std::vector<const Transaction*> result;
    
    for (const auto& transaction : m_paymentGateway.getTransactions()) {
        if (transaction->getMerchant().getName() == merchantId) {
            result.push_back(transaction.get());
        }
    }
    
    return result;
}

std::unique_ptr<PaymentMethod> PaymentGatewayFacade::createPaymentMethod(
    const std::string& type,
    const std::vector<std::string>& details) const {
    
    if (type == "Credit Card") {
        if (details.size() >= 4) {
            return PaymentMethodFactory::createCreditCard(
                details[0], details[1], details[2], details[3]);
        }
    } else if (type == "Debit Card") {
        if (details.size() >= 4) {
            return PaymentMethodFactory::createDebitCard(
                details[0], details[1], details[2], details[3]);
        }
    } else if (type == "Digital Wallet") {
        if (details.size() >= 2) {
            return PaymentMethodFactory::createDigitalWallet(
                details[0], details[1]);
        }
    }
    
    std::cerr << "Invalid payment method type or insufficient details" << std::endl;
    return nullptr;
}
