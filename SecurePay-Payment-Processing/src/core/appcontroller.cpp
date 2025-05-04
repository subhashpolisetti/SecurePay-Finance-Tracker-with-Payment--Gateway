#include "appcontroller.h"
#include <iostream>

AppController::AppController() : m_paymentGateway(std::make_unique<PaymentGateway>()) {
    std::cout << "AppController initialized" << std::endl;
    
    m_paymentGateway->addObserver(this);
    
    // Add sample customers
    addCustomer(Customer("Alice Smith", "alice@example.com", "123 Main St, San Francisco, CA"));
    addCustomer(Customer("Bob Johnson", "bob@example.com", "456 Oak Ave, New York, NY"));
    
    // Add sample merchants
    addMerchant(Merchant("Acme Store", "acme@example.com", "789 Market St, San Francisco, CA"));
    addMerchant(Merchant("XYZ Electronics", "xyz@example.com", "101 Tech Blvd, San Jose, CA"));
}

AppController::~AppController() {
    if (m_paymentGateway) {
        m_paymentGateway->removeObserver(this);
    }
}

void AppController::addCustomer(const Customer& customer) {
    m_customers.push_back(customer);
    std::cout << "Added customer: " << customer.getName() << std::endl;
}

const std::vector<Customer>& AppController::getCustomers() const {
    return m_customers;
}

void AppController::addMerchant(const Merchant& merchant) {
    m_merchants.push_back(merchant);
    std::cout << "Added merchant: " << merchant.getName() << std::endl;
}

const std::vector<Merchant>& AppController::getMerchants() const {
    return m_merchants;
}

std::unique_ptr<Transaction> AppController::createTransaction(
    const Customer& customer,
    const Merchant& merchant,
    const std::string& paymentMethodType,
    const std::string& paymentDetails1,
    const std::string& paymentDetails2,
    const std::string& paymentDetails3,
    const std::string& paymentDetails4,
    double amount) {
    
    auto paymentMethod = createPaymentMethod(
        paymentMethodType, 
        paymentDetails1, 
        paymentDetails2, 
        paymentDetails3, 
        paymentDetails4);
    
    if (!paymentMethod) {
        std::cerr << "Failed to create payment method" << std::endl;
        return nullptr;
    }
    
    return TransactionFactory::createTransaction(
        customer, merchant, std::move(paymentMethod), amount);
}

void AppController::processTransaction(std::unique_ptr<Transaction> transaction) {
    if (m_paymentGateway && transaction) {
        m_paymentGateway->processTransaction(std::move(transaction));
    }
}

const std::vector<std::unique_ptr<Transaction>>& AppController::getTransactionHistory() const {
    return m_paymentGateway->getTransactions();
}

void AppController::onTransactionUpdated(const Transaction& transaction) {
    std::cout << "Transaction updated: " << transaction.getTransactionId() 
              << " - Status: " << Transaction::statusToString(transaction.getStatus()) 
              << std::endl;
    
    if (m_transactionUpdateCallback) {
        m_transactionUpdateCallback(transaction);
    }
}

void AppController::setTransactionUpdateCallback(std::function<void(const Transaction&)> callback) {
    m_transactionUpdateCallback = callback;
}

std::unique_ptr<PaymentMethod> AppController::createPaymentMethod(
    const std::string& paymentMethodType,
    const std::string& details1,
    const std::string& details2,
    const std::string& details3,
    const std::string& details4) {
    
    if (paymentMethodType == "Credit Card") {
        return PaymentMethodFactory::createCreditCard(
            details1,
            details2,
            details3,
            details4
        );
    } else if (paymentMethodType == "Debit Card") {
        return PaymentMethodFactory::createDebitCard(
            details1,
            details2,
            details3,
            details4
        );
    } else if (paymentMethodType == "Digital Wallet") {
        return PaymentMethodFactory::createDigitalWallet(
            details1,
            details2
        );
    }
    
    std::cerr << "Unknown payment method type: " << paymentMethodType << std::endl;
    return nullptr;
}
