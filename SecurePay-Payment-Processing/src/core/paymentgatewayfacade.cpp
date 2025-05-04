#include "paymentgatewayfacade.h"
#include <iostream>
#include <algorithm>

PaymentGatewayFacade::PaymentGatewayFacade(
    PaymentGateway& paymentGateway, Bank& bank, FraudSystem& fraudSystem)
    : m_paymentGateway(paymentGateway), m_bank(bank), m_fraudSystem(fraudSystem) {
}

std::string PaymentGatewayFacade::processPayment(
    const Customer& customer,
    const Merchant& merchant,
    const std::string& paymentMethodType,
    const std::vector<std::string>& paymentDetails,
    double amount) {
    
    // Create payment method
    auto paymentMethod = createPaymentMethod(paymentMethodType, paymentDetails);
    if (!paymentMethod) {
        std::cerr << "Failed to create payment method" << std::endl;
        return "";
    }
    
    // Create transaction
    auto transaction = TransactionFactory::createTransaction(
        customer, merchant, std::move(paymentMethod), amount);
    
    // Process transaction
    std::string transactionId = transaction->getTransactionId();
    m_paymentGateway.processTransaction(std::move(transaction));
    
    // Since the transaction is moved into the payment gateway, we can't access it anymore
    // We'll return the transaction ID that we saved before processing
    return transactionId;
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
