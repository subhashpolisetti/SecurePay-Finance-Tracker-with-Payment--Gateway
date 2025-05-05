#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <map>

/**
 * @class Customer
 * @brief Represents a customer in the payment system
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * customer data representation and basic operations.
 */
class Customer {
public:
    /**
     * @brief Default constructor
     */
    Customer();
    
    /**
     * @brief Parameterized constructor
     * @param name The customer's name
     * @param email The customer's email
     * @param billingAddress The customer's billing address
     */
    Customer(const std::string& name, const std::string& email, const std::string& billingAddress);
    
    /**
     * @brief Get the customer's name
     * @return The customer's name
     */
    std::string getName() const;
    
    /**
     * @brief Get the customer's email
     * @return The customer's email
     */
    std::string getEmail() const;
    
    /**
     * @brief Get the customer's billing address
     * @return The customer's billing address
     */
    std::string getBillingAddress() const;
    
    /**
     * @brief Set the customer's name
     * @param name The new name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Set the customer's email
     * @param email The new email
     */
    void setEmail(const std::string& email);
    
    /**
     * @brief Set the customer's billing address
     * @param billingAddress The new billing address
     */
    void setBillingAddress(const std::string& billingAddress);
    
    /**
     * @brief Set the balance for a payment method
     * @param method The payment method type (e.g., "Credit Card", "Debit Card", "Digital Wallet")
     * @param amount The balance amount
     */
    void setBalance(const std::string& method, double amount);
    
    /**
     * @brief Get the balance for a payment method
     * @param method The payment method type
     * @return The balance amount, or 0.0 if the method doesn't exist
     */
    double getBalance(const std::string& method) const;
    
    /**
     * @brief Deduct an amount from a payment method balance
     * @param method The payment method type
     * @param amount The amount to deduct
     * @return True if successful, false if insufficient funds
     */
    bool deduct(const std::string& method, double amount);
    
    /**
     * @brief Refund an amount to a payment method balance
     * @param method The payment method type
     * @param amount The amount to refund
     */
    void refund(const std::string& method, double amount);
    
    /**
     * @brief Get all payment method balances
     * @return Map of payment method types to balances
     */
    const std::map<std::string, double>& getAllBalances() const;
    
private:
    std::string m_name;
    std::string m_email;
    std::string m_billingAddress;
    std::map<std::string, double> m_balances;
};

#endif // CUSTOMER_H
