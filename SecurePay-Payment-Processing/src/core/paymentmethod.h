#ifndef PAYMENTMETHOD_H
#define PAYMENTMETHOD_H

#include <string>
#include <memory>

/**
 * @class PaymentMethod
 * @brief Abstract base class for payment methods (Strategy pattern)
 * 
 * This class follows the Strategy Pattern to encapsulate different payment
 * processing algorithms.
 */
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    
    /**
     * @brief Process a payment
     * @param amount The payment amount
     * @return True if processing was successful, false otherwise
     */
    virtual bool process(double amount) const = 0;
    
    /**
     * @brief Get the payment method type
     * @return The payment method type
     */
    virtual std::string getType() const = 0;
    
    /**
     * @brief Get the payment method details
     * @return The payment method details
     */
    virtual std::string getDetails() const = 0;
    
    /**
     * @brief Create a clone of this payment method
     * @return A unique pointer to the cloned payment method
     */
    virtual PaymentMethod* clone() const = 0;
};


/**
 * @class CreditCard
 * @brief Credit card payment method
 */
class CreditCard : public PaymentMethod {
public:
    CreditCard(const std::string& cardNumber, const std::string& cardholderName, 
               const std::string& expiryDate, const std::string& cvv);
    
    bool process(double amount) const override;
    std::string getType() const override;
    std::string getDetails() const override;
    PaymentMethod* clone() const override;
    
private:
    std::string m_cardNumber;
    std::string m_cardholderName;
    std::string m_expiryDate;
    std::string m_cvv;
};


/**
 * @class DebitCard
 * @brief Debit card payment method
 */
class DebitCard : public PaymentMethod {
public:
    DebitCard(const std::string& cardNumber, const std::string& cardholderName, 
              const std::string& expiryDate, const std::string& cvv);
    
    bool process(double amount) const override;
    std::string getType() const override;
    std::string getDetails() const override;
    PaymentMethod* clone() const override;
    
private:
    std::string m_cardNumber;
    std::string m_cardholderName;
    std::string m_expiryDate;
    std::string m_cvv;
};


/**
 * @class DigitalWallet
 * @brief Digital wallet payment method
 */
class DigitalWallet : public PaymentMethod {
public:
    DigitalWallet(const std::string& walletId, const std::string& email);
    
    bool process(double amount) const override;
    std::string getType() const override;
    std::string getDetails() const override;
    PaymentMethod* clone() const override;
    
private:
    std::string m_walletId;
    std::string m_email;
};


class PaymentMethodFactory {
public:
    static std::unique_ptr<PaymentMethod> createCreditCard(
        const std::string& cardNumber, const std::string& cardholderName,
        const std::string& expiryDate, const std::string& cvv);
        
    static std::unique_ptr<PaymentMethod> createDebitCard(
        const std::string& cardNumber, const std::string& cardholderName,
        const std::string& expiryDate, const std::string& cvv);
        
    static std::unique_ptr<PaymentMethod> createDigitalWallet(
        const std::string& walletId, const std::string& email);
};

#endif
