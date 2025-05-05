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
 * @class Result
 * @brief Generic result class for operations
 * 
 * This class provides a standardized way to return results from operations,
 * including success/failure status, error codes, and error messages.
 */
template <typename T>
class Result {
private:
    bool m_success;
    std::string m_errorCode;
    std::string m_errorMessage;
    T m_data;
    
public:
    /**
     * @brief Create a success result
     * @param data The result data
     * @return A success result
     */
    static Result<T> success(const T& data) {
        Result<T> result;
        result.m_success = true;
        result.m_data = data;
        return result;
    }
    
    /**
     * @brief Create a failure result
     * @param errorCode The error code
     * @param errorMessage The error message
     * @return A failure result
     */
    static Result<T> failure(const std::string& errorCode, const std::string& errorMessage) {
        Result<T> result;
        result.m_success = false;
        result.m_errorCode = errorCode;
        result.m_errorMessage = errorMessage;
        return result;
    }
    
    /**
     * @brief Check if the result is successful
     * @return True if successful, false otherwise
     */
    bool isSuccess() const { return m_success; }
    
    /**
     * @brief Get the error code
     * @return The error code
     */
    const std::string& getErrorCode() const { return m_errorCode; }
    
    /**
     * @brief Get the error message
     * @return The error message
     */
    const std::string& getErrorMessage() const { return m_errorMessage; }
    
    /**
     * @brief Get the result data
     * @return The result data
     */
    const T& getData() const { return m_data; }
};

/**
 * @brief Specialization for void return type
 */
template <>
class Result<void> {
private:
    bool m_success;
    std::string m_errorCode;
    std::string m_errorMessage;
    
public:
    /**
     * @brief Create a success result
     * @return A success result
     */
    static Result<void> success() {
        Result<void> result;
        result.m_success = true;
        return result;
    }
    
    /**
     * @brief Create a failure result
     * @param errorCode The error code
     * @param errorMessage The error message
     * @return A failure result
     */
    static Result<void> failure(const std::string& errorCode, const std::string& errorMessage) {
        Result<void> result;
        result.m_success = false;
        result.m_errorCode = errorCode;
        result.m_errorMessage = errorMessage;
        return result;
    }
    
    /**
     * @brief Check if the result is successful
     * @return True if successful, false otherwise
     */
    bool isSuccess() const { return m_success; }
    
    /**
     * @brief Get the error code
     * @return The error code
     */
    const std::string& getErrorCode() const { return m_errorCode; }
    
    /**
     * @brief Get the error message
     * @return The error message
     */
    const std::string& getErrorMessage() const { return m_errorMessage; }
};

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
     * @return Result containing the transaction ID if successful
     */
    Result<std::string> processPayment(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::vector<std::string>& paymentDetails,
        double amount);
    
    /**
     * @brief Process a payment with an idempotency key
     * @param customer The customer making the payment
     * @param merchant The merchant receiving the payment
     * @param paymentMethodType The type of payment method
     * @param paymentDetails Payment method details
     * @param amount The payment amount
     * @param idempotencyKey The idempotency key
     * @return Result containing the transaction ID if successful
     */
    Result<std::string> processPaymentWithIdempotencyKey(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::vector<std::string>& paymentDetails,
        double amount,
        const std::string& idempotencyKey);
    
    /**
     * @brief Authorize a payment (hold funds without capturing)
     * @param customer The customer making the payment
     * @param merchant The merchant receiving the payment
     * @param paymentMethodType The type of payment method
     * @param paymentDetails Payment method details
     * @param amount The payment amount
     * @return Result containing the transaction ID if successful
     */
    Result<std::string> authorizePayment(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::vector<std::string>& paymentDetails,
        double amount);
    
    /**
     * @brief Capture a previously authorized payment
     * @param transactionId The ID of the transaction to capture
     * @param amount The amount to capture (0 for full amount)
     * @return Result indicating success or failure
     */
    Result<void> capturePayment(const std::string& transactionId, double amount = 0);
    
    /**
     * @brief Void a previously authorized payment
     * @param transactionId The ID of the transaction to void
     * @return Result indicating success or failure
     */
    Result<void> voidPayment(const std::string& transactionId);
    
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
