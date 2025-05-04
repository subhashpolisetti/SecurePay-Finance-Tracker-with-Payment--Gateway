#ifndef PAYMENTGATEWAYFACADE_H
#define PAYMENTGATEWAYFACADE_H

#include <memory>
#include <string>
#include <vector>
#include "paymentgateway.h"
#include "bank.h"
#include "fraudsystem.h"
#include "transaction.h"
#include "customer.h"
#include "merchant.h"
#include "paymentmethod.h"

/**
 * @class PaymentGatewayFacade
 * @brief Simplified interface to the PaymentGateway (Façade Pattern)
 * 
 * This class follows the Façade Pattern to provide a simplified interface
 * to the complex subsystem of payment processing, hiding the interactions
 * between PaymentGateway, Bank, and FraudSystem.
 */
class PaymentGatewayFacade {
public:
    /**
     * @brief Constructor
     * @param paymentGateway Reference to the payment gateway
     * @param bank Reference to the bank
     * @param fraudSystem Reference to the fraud system
     */
    PaymentGatewayFacade(PaymentGateway& paymentGateway, Bank& bank, FraudSystem& fraudSystem);
    
    /**
     * @brief Process a payment
     * @param customer The customer making the payment
     * @param merchant The merchant receiving the payment
     * @param paymentMethodType The type of payment method
     * @param paymentDetails Payment method details
     * @param amount The payment amount
     * @return The transaction ID if successful, empty string otherwise
     */
    std::string processPayment(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::vector<std::string>& paymentDetails,
        double amount);
    
    /**
     * @brief Get a transaction by ID
     * @param transactionId The transaction ID
     * @return Pointer to the transaction, or nullptr if not found
     */
    const Transaction* getTransaction(const std::string& transactionId) const;
    
    /**
     * @brief Get all transactions
     * @return Vector of transactions
     */
    std::vector<const Transaction*> getAllTransactions() const;
    
    /**
     * @brief Get transactions for a customer
     * @param customerId The customer ID
     * @return Vector of transactions
     */
    std::vector<const Transaction*> getTransactionsForCustomer(const std::string& customerId) const;
    
    /**
     * @brief Get transactions for a merchant
     * @param merchantId The merchant ID
     * @return Vector of transactions
     */
    std::vector<const Transaction*> getTransactionsForMerchant(const std::string& merchantId) const;
    
private:
    PaymentGateway& m_paymentGateway;
    Bank& m_bank;
    FraudSystem& m_fraudSystem;
    
    /**
     * @brief Create a payment method from type and details
     * @param type The payment method type
     * @param details Payment method details
     * @return Unique pointer to the payment method
     */
    std::unique_ptr<PaymentMethod> createPaymentMethod(
        const std::string& type,
        const std::vector<std::string>& details) const;
};

#endif // PAYMENTGATEWAYFACADE_H
