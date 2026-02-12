#ifndef PAYMENTMETHOD_H
#define PAYMENTMETHOD_H

#include <string>
#include <memory>
#include <vector>

/**
 * @brief Luhn (mod 10) checksum used by card networks to catch mistyped numbers
 * @param cardNumber Digits only, 13-19 characters
 * @return True if the number has a valid length, contains only digits and passes the checksum
 */
bool passesLuhnCheck(const std::string& cardNumber);

/**
 * @brief Check a card expiry date ("MM/YY" or "MMYY") against a given month
 *
 * Cards remain valid through the last day of their expiry month.
 * @param expiryDate The expiry date as entered on the card
 * @param currentYear Four-digit year to compare against
 * @param currentMonth Month (1-12) to compare against
 * @return True if the date is well-formed and not in the past
 */
bool isExpiryDateValid(const std::string& expiryDate, int currentYear, int currentMonth);

/**
 * @brief Check a card expiry date against today's date
 */
bool isExpiryDateValid(const std::string& expiryDate);

/**
 * @brief Check that a CVV is three or four digits
 */
bool isCvvValid(const std::string& cvv);

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
     * @brief Check that the payment details are well-formed before authorization
     * @return True if the details can be sent for authorization
     */
    virtual bool isValid() const = 0;
    
    /**
     * @brief Details that are safe to persist, in the order the factory accepts them
     *
     * Cards never expose the full number or the CVV here, only the last four digits.
     * @return Up to four detail strings
     */
    virtual std::vector<std::string> getStorableDetails() const = 0;
    
    /**
     * @brief Create a clone of this payment method
     * @return A unique pointer to the cloned payment method
     */
    virtual std::unique_ptr<PaymentMethod> clone() const = 0;
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
    bool isValid() const override;
    std::vector<std::string> getStorableDetails() const override;
    std::unique_ptr<PaymentMethod> clone() const override;
    
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
    bool isValid() const override;
    std::vector<std::string> getStorableDetails() const override;
    std::unique_ptr<PaymentMethod> clone() const override;
    
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
    bool isValid() const override;
    std::vector<std::string> getStorableDetails() const override;
    std::unique_ptr<PaymentMethod> clone() const override;
    
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
