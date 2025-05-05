#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include "customer.h"
#include "merchant.h"
#include "transaction.h"
#include "paymentmethod.h"
#include "paymentgateway.h"
#include "paymentgatewayfacade.h"
#include "enhancedfraudsystem.h"
#include "cardtoken.h"
#include "sqlitedatamanager.h"

/**
 * @class AppController
 * @brief Controller class for the application
 * 
 * This class follows the Controller pattern to manage the application's
 * business logic and coordinate between the UI and the model.
 * It also implements the Singleton pattern to ensure only one instance exists.
 */
class AppController : public TransactionObserver {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static AppController& getInstance() {
        static AppController instance;
        return instance;
    }
    
    /**
     * @brief Destructor
     */
    ~AppController();
    
    /**
     * @brief Delete copy constructor
     */
    AppController(const AppController&) = delete;
    
    /**
     * @brief Delete assignment operator
     */
    AppController& operator=(const AppController&) = delete;
    
    /**
     * @brief Add a customer
     * @param customer The customer to add
     * @return Reference to the added customer
     */
    const Customer& addCustomer(const Customer& customer);
    
    /**
     * @brief Get all customers
     * @return Vector of customers
     */
    const std::vector<Customer>& getCustomers() const;
    
    /**
     * @brief Get mutable customers
     * @return Mutable vector of customers
     */
    std::vector<Customer>& getCustomersMutable();
    
    /**
     * @brief Find a customer by user ID
     * @param userId The user ID
     * @return Pointer to the customer, or nullptr if not found
     */
    const Customer* findCustomerByUserId(const std::string& userId) const;
    
    /**
     * @brief Find a customer by username or email
     * @param usernameOrEmail The username or email
     * @return Pointer to the customer, or nullptr if not found
     */
    const Customer* findCustomerByUsernameOrEmail(const std::string& usernameOrEmail) const;
    
    /**
     * @brief Authenticate a customer
     * @param usernameOrEmail The username or email
     * @param pin The PIN
     * @return Pointer to the authenticated customer, or nullptr if authentication failed
     */
    const Customer* authenticateCustomer(const std::string& usernameOrEmail, const std::string& pin);
    
    /**
     * @brief Get the currently authenticated customer
     * @return Pointer to the authenticated customer, or nullptr if not authenticated
     */
    const Customer* getAuthenticatedCustomer() const;
    
    /**
     * @brief Set the authenticated customer
     * @param customer Pointer to the customer to set as authenticated
     */
    void setAuthenticatedCustomer(const Customer* customer);
    
    /**
     * @brief Log out the currently authenticated customer
     */
    void logoutCustomer();
    
    /**
     * @brief Check if a customer is authenticated
     * @return True if a customer is authenticated, false otherwise
     */
    bool isCustomerAuthenticated() const;
    
    /**
     * @brief Add a merchant
     * @param merchant The merchant to add
     */
    void addMerchant(const Merchant& merchant);
    
    /**
     * @brief Get all merchants
     * @return Vector of merchants
     */
    const std::vector<Merchant>& getMerchants() const;
    
    /**
     * @brief Create a transaction
     * @param customer The customer
     * @param merchant The merchant
     * @param paymentMethodType The payment method type
     * @param paymentDetails1 First payment method detail
     * @param paymentDetails2 Second payment method detail
     * @param paymentDetails3 Third payment method detail
     * @param paymentDetails4 Fourth payment method detail
     * @param amount The transaction amount
     * @return Unique pointer to the created transaction
     */
    std::unique_ptr<Transaction> createTransaction(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::string& paymentDetails1,
        const std::string& paymentDetails2,
        const std::string& paymentDetails3,
        const std::string& paymentDetails4,
        double amount);
    
    /**
     * @brief Process a transaction
     * @param transaction The transaction to process
     */
    void processTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Process a transaction with an idempotency key
     * @param transaction The transaction to process
     * @param idempotencyKey The idempotency key
     * @return The transaction ID
     */
    std::string processTransactionWithIdempotencyKey(
        std::unique_ptr<Transaction> transaction,
        const std::string& idempotencyKey);
    
    /**
     * @brief Authorize a transaction (hold funds without capturing)
     * @param transaction The transaction to authorize
     * @return True if authorization was successful, false otherwise
     */
    bool authorizeTransaction(std::unique_ptr<Transaction> transaction);
    
    /**
     * @brief Capture a previously authorized transaction
     * @param transactionId The ID of the transaction to capture
     * @param amount The amount to capture (0 for full amount)
     * @return True if capture was successful, false otherwise
     */
    bool captureTransaction(const std::string& transactionId, double amount = 0);
    
    /**
     * @brief Void a previously authorized transaction
     * @param transactionId The ID of the transaction to void
     * @return True if void was successful, false otherwise
     */
    bool voidTransaction(const std::string& transactionId);
    
    /**
     * @brief Get transaction history
     * @return Vector of transactions
     */
    const std::vector<std::unique_ptr<Transaction>>& getTransactionHistory() const;
    
    /**
     * @brief Find a transaction by ID
     * @param transactionId The transaction ID
     * @return Pointer to the transaction, or nullptr if not found
     */
    Transaction* findTransaction(const std::string& transactionId);
    
    /**
     * @brief Add a card token
     * @param cardToken The card token to add
     */
    void addCardToken(std::unique_ptr<CardToken> cardToken);
    
    /**
     * @brief Get card tokens for a customer
     * @param customerId The customer ID
     * @return Vector of card tokens
     */
    std::vector<const CardToken*> getCardTokensForCustomer(const std::string& customerId) const;
    
    /**
     * @brief Delete a card token
     * @param token The token to delete
     * @return True if deletion was successful, false otherwise
     */
    bool deleteCardToken(const std::string& token);
    
    /**
     * @brief Transaction update callback
     * @param transaction The updated transaction
     */
    void onTransactionUpdated(const Transaction& transaction) override;
    
    /**
     * @brief Set transaction update callback
     * @param callback The callback function
     */
    void setTransactionUpdateCallback(std::function<void(const Transaction&)> callback);
    
    /**
     * @brief Save all data to the database
     * @return True if save was successful, false otherwise
     */
    bool saveAllData();
    
    /**
     * @brief Load all data from the database
     * @return True if load was successful, false otherwise
     */
    bool loadAllData();

private:
    /**
     * @brief Private constructor for singleton
     */
    AppController();
    
    /**
     * @brief Vector of customers
     */
    std::vector<Customer> m_customers;
    
    /**
     * @brief Currently authenticated customer
     */
    const Customer* m_authenticatedCustomer;
    
    /**
     * @brief Vector of merchants
     */
    std::vector<Merchant> m_merchants;
    
    /**
     * @brief Payment gateway
     */
    std::unique_ptr<PaymentGateway> m_paymentGateway;
    
    /**
     * @brief Payment gateway facade
     */
    std::unique_ptr<PaymentGatewayFacade> m_paymentGatewayFacade;
    
    /**
     * @brief Transaction update callback
     */
    std::function<void(const Transaction&)> m_transactionUpdateCallback;
    
    /**
     * @brief Data manager for persistence
     */
    std::unique_ptr<SQLiteDataManager> m_dataManager;
    
    /**
     * @brief Create a payment method
     * @param paymentMethodType The payment method type
     * @param details1 First payment method detail
     * @param details2 Second payment method detail
     * @param details3 Third payment method detail
     * @param details4 Fourth payment method detail
     * @return Unique pointer to the created payment method
     */
    std::unique_ptr<PaymentMethod> createPaymentMethod(
        const std::string& paymentMethodType,
        const std::string& details1,
        const std::string& details2,
        const std::string& details3,
        const std::string& details4);
};

#endif
