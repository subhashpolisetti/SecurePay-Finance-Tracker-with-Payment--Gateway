#include "appcontroller.h"
#include "refundmanager.h"
#include "bank.h"
#include "fraudsystem.h"
#include <iostream>
#include <algorithm>

AppController::AppController() 
    : m_authenticatedCustomer(nullptr),
      m_paymentGateway(std::make_unique<PaymentGateway>()) {
    
    // Initialize the payment gateway facade
    Bank& bank = Bank::getInstance();
    FraudSystem& fraudSystem = FraudSystem::getInstance();
    m_paymentGatewayFacade = std::make_unique<PaymentGatewayFacade>(*m_paymentGateway, bank, fraudSystem);
    
    // Initialize the enhanced fraud system
    EnhancedFraudSystem::getInstance();
    
    // Register as an observer for transaction updates
    m_paymentGateway->addObserver(this);
    
    // Initialize the data manager
    std::string dbPath = "securepay.db"; // Database file path
    m_dataManager = std::make_unique<SQLiteDataManager>(dbPath);
    
    if (m_dataManager->initialize()) {
        // Try to load data from the database
        if (loadAllData()) {
            // Data loaded successfully
        } else {
            // No data found, use sample data
            
            // Add sample customers
            addCustomer(Customer("Alice Smith", "alice@example.com", "123 Main St, San Francisco, CA"));
            addCustomer(Customer("Bob Johnson", "bob@example.com", "456 Oak Ave, New York, NY"));
            
            // Add sample merchants
            addMerchant(Merchant("Acme Store", "acme@example.com", "789 Market St, San Francisco, CA"));
            addMerchant(Merchant("XYZ Electronics", "xyz@example.com", "101 Tech Blvd, San Jose, CA"));
            
            // Save the sample data to the database
            saveAllData();
        }
    } else {
        std::cerr << "Failed to initialize database, using in-memory storage only" << std::endl;
        
        // Add sample customers
        addCustomer(Customer("Alice Smith", "alice@example.com", "123 Main St, San Francisco, CA"));
        addCustomer(Customer("Bob Johnson", "bob@example.com", "456 Oak Ave, New York, NY"));
        
        // Add sample merchants
        addMerchant(Merchant("Acme Store", "acme@example.com", "789 Market St, San Francisco, CA"));
        addMerchant(Merchant("XYZ Electronics", "xyz@example.com", "101 Tech Blvd, San Jose, CA"));
    }
    
    // Set the customers vector in the RefundManager
    RefundManager::getInstance().setCustomers(&m_customers);
}

AppController::~AppController() {
    if (m_paymentGateway) {
        m_paymentGateway->removeObserver(this);
    }
    
    // Save all data before shutting down
    if (m_dataManager) {
        saveAllData();
    }
}

bool AppController::saveAllData() {
    if (!m_dataManager) {
        return false;
    }
    
    bool success = true;
    
    // Save customers
    for (const auto& customer : m_customers) {
        if (!m_dataManager->saveCustomer(customer)) {
            std::cerr << "Failed to save customer: " << customer.getName() << std::endl;
            success = false;
        }
    }
    
    // Save merchants
    for (const auto& merchant : m_merchants) {
        if (!m_dataManager->saveMerchant(merchant)) {
            std::cerr << "Failed to save merchant: " << merchant.getName() << std::endl;
            success = false;
        }
    }
    
    // Save transactions
    for (const auto& transaction : m_paymentGateway->getTransactions()) {
        if (!m_dataManager->saveTransaction(*transaction)) {
            std::cerr << "Failed to save transaction: " << transaction->getTransactionId() << std::endl;
            success = false;
        }
    }
    
    // Save card tokens for all customers
    CardManager& cardManager = CardManager::getInstance();
    for (const auto& customer : m_customers) {
        auto cardTokens = cardManager.getCardTokensForCustomer(customer.getName());
        for (const auto* token : cardTokens) {
            if (!m_dataManager->saveCardToken(*token)) {
                std::cerr << "Failed to save card token: " << token->getToken() << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

bool AppController::loadAllData() {
    if (!m_dataManager) {
        return false;
    }
    
    bool success = true;
    
    // Load customers
    m_customers = m_dataManager->loadCustomers();
    if (m_customers.empty()) {
        success = false;
    }
    
    // Load merchants
    m_merchants = m_dataManager->loadMerchants();
    if (m_merchants.empty()) {
        success = false;
    }
    
    // Load transactions
    auto transactions = m_dataManager->loadTransactions(m_customers, m_merchants);
    if (transactions.empty()) {
        success = false;
    } else {
        // Add transactions to the payment gateway without processing them again
        for (auto& transaction : transactions) {
            // Store the transaction directly without processing
            m_paymentGateway->storeTransaction(std::move(transaction));
        }
    }
    
    // Load card tokens
    auto cardTokens = m_dataManager->loadCardTokens();
    if (cardTokens.empty()) {
        success = false;
    } else {
        // Add card tokens to the card manager
        CardManager& cardManager = CardManager::getInstance();
        for (auto& cardToken : cardTokens) {
            cardManager.addCardToken(std::move(cardToken));
        }
    }
    
    return success;
}

const Customer& AppController::addCustomer(const Customer& customer) {
    m_customers.push_back(customer);
    
    // Save the customer to the database
    if (m_dataManager) {
        m_dataManager->saveCustomer(customer);
    }
    
    // Return a reference to the newly added customer
    return m_customers.back();
}

const Customer* AppController::findCustomerByUserId(const std::string& userId) const {
    auto it = std::find_if(m_customers.begin(), m_customers.end(),
                          [&userId](const Customer& c) { return c.getUserId() == userId; });
    
    if (it != m_customers.end()) {
        return &(*it);
    }
    
    return nullptr;
}

const Customer* AppController::findCustomerByUsernameOrEmail(const std::string& usernameOrEmail) const {
    auto it = std::find_if(m_customers.begin(), m_customers.end(),
                          [&usernameOrEmail](const Customer& c) { 
                              return c.getUsername() == usernameOrEmail || c.getEmail() == usernameOrEmail; 
                          });
    
    if (it != m_customers.end()) {
        return &(*it);
    }
    
    return nullptr;
}

const Customer* AppController::authenticateCustomer(const std::string& usernameOrEmail, const std::string& pin) {
    for (const auto& customer : m_customers) {
        if (customer.authenticate(usernameOrEmail, pin)) {
            m_authenticatedCustomer = &customer;
            return m_authenticatedCustomer;
        }
    }
    
    return nullptr;
}

const Customer* AppController::getAuthenticatedCustomer() const {
    return m_authenticatedCustomer;
}

void AppController::setAuthenticatedCustomer(const Customer* customer) {
    m_authenticatedCustomer = customer;
}

void AppController::logoutCustomer() {
    m_authenticatedCustomer = nullptr;
}

bool AppController::isCustomerAuthenticated() const {
    return m_authenticatedCustomer != nullptr;
}

const std::vector<Customer>& AppController::getCustomers() const {
    return m_customers;
}

std::vector<Customer>& AppController::getCustomersMutable() {
    return m_customers;
}

void AppController::addMerchant(const Merchant& merchant) {
    m_merchants.push_back(merchant);
    
    // Save the merchant to the database
    if (m_dataManager) {
        m_dataManager->saveMerchant(merchant);
    }
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
        // Store the customer name and payment method type for later lookup
        std::string customerName = transaction->getCustomer().getName();
        std::string paymentMethodType = transaction->getPaymentMethod().getType();
        double amount = transaction->getAmount();
        std::string transactionId = transaction->getTransactionId();
        
        // Process the transaction
        m_paymentGateway->processTransaction(std::move(transaction));
        
        // Find the customer by name
        for (auto& customer : m_customers) {
            if (customer.getName() == customerName) {
                // Find the processed transaction
                for (const auto& tx : m_paymentGateway->getTransactions()) {
                    if (tx->getTransactionId() == transactionId) {
                        // If the transaction was approved, deduct funds from the customer's account
                        if (tx->getStatus() == TransactionStatus::APPROVED) {
                            // Check if customer has sufficient funds
                            if (customer.getBalance(paymentMethodType) >= amount) {
                                // Deduct funds from the customer's account
                                bool deducted = customer.deduct(paymentMethodType, amount);
                                
                                if (deducted) {
                                    std::cout << "Successfully deducted $" << amount 
                                              << " from " << customerName << "'s " 
                                              << paymentMethodType << " account" << std::endl;
                                } else {
                                    std::cerr << "Failed to deduct funds from customer account" << std::endl;
                                }
                            } else {
                                std::cerr << "Insufficient funds in customer account" << std::endl;
                            }
                        }
                        
                        // Save the transaction and updated customer to the database
                        if (m_dataManager) {
                            m_dataManager->saveTransaction(*tx);
                            m_dataManager->saveCustomer(customer);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}

std::string AppController::processTransactionWithIdempotencyKey(
    std::unique_ptr<Transaction> transaction,
    const std::string& idempotencyKey) {
    
    if (m_paymentGateway && transaction) {
        // Store the customer name and payment method type for later lookup
        std::string customerName = transaction->getCustomer().getName();
        std::string paymentMethodType = transaction->getPaymentMethod().getType();
        double amount = transaction->getAmount();
        
        // Process the transaction with idempotency key
        std::string transactionId = m_paymentGateway->processTransactionWithIdempotencyKey(
            std::move(transaction), idempotencyKey);
        
        // Find the customer by name
        for (auto& customer : m_customers) {
            if (customer.getName() == customerName) {
                // Find the transaction with the matching ID
                Transaction* tx = findTransaction(transactionId);
                if (tx && tx->getStatus() == TransactionStatus::APPROVED) {
                    // Check if customer has sufficient funds
                    if (customer.getBalance(paymentMethodType) >= amount) {
                        // Deduct funds from the customer's account
                        bool deducted = customer.deduct(paymentMethodType, amount);
                        
                        if (deducted) {
                            std::cout << "Successfully deducted $" << amount 
                                      << " from " << customerName << "'s " 
                                      << paymentMethodType << " account" << std::endl;
                        } else {
                            std::cerr << "Failed to deduct funds from customer account" << std::endl;
                        }
                    } else {
                        std::cerr << "Insufficient funds in customer account" << std::endl;
                    }
                    
                    // Save the updated customer to the database
                    if (m_dataManager) {
                        m_dataManager->saveCustomer(customer);
                    }
                }
                break;
            }
        }
        
        return transactionId;
    }
    
    return "";
}

bool AppController::authorizeTransaction(std::unique_ptr<Transaction> transaction) {
    if (m_paymentGateway && transaction) {
        return m_paymentGateway->authorizeTransaction(std::move(transaction));
    }
    
    return false;
}

bool AppController::captureTransaction(const std::string& transactionId, double amount) {
    if (m_paymentGateway) {
        return m_paymentGateway->captureTransaction(transactionId, amount);
    }
    
    return false;
}

bool AppController::voidTransaction(const std::string& transactionId) {
    if (m_paymentGateway) {
        return m_paymentGateway->voidTransaction(transactionId);
    }
    
    return false;
}

Transaction* AppController::findTransaction(const std::string& transactionId) {
    if (m_paymentGateway) {
        return m_paymentGateway->findTransaction(transactionId);
    }
    
    return nullptr;
}

void AppController::addCardToken(std::unique_ptr<CardToken> cardToken) {
    if (cardToken) {
        // Save the card token to the database before adding it to the card manager
        if (m_dataManager) {
            m_dataManager->saveCardToken(*cardToken);
        }
        
        CardManager::getInstance().addCardToken(std::move(cardToken));
    }
}

std::vector<const CardToken*> AppController::getCardTokensForCustomer(const std::string& customerId) const {
    return CardManager::getInstance().getCardTokensForCustomer(customerId);
}

bool AppController::deleteCardToken(const std::string& token) {
    bool success = CardManager::getInstance().deleteCardToken(token);
    
    // Delete the card token from the database
    if (success && m_dataManager) {
        m_dataManager->deleteCardToken(token);
    }
    
    return success;
}

const std::vector<std::unique_ptr<Transaction>>& AppController::getTransactionHistory() const {
    return m_paymentGateway->getTransactions();
}

void AppController::onTransactionUpdated(const Transaction& transaction) {
    // Save the updated transaction to the database
    if (m_dataManager) {
        m_dataManager->saveTransaction(transaction);
    }
    
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
