#ifndef CARDTOKEN_H
#define CARDTOKEN_H

#include <string>
#include <memory>
#include <vector>
#include <random>

/**
 * @enum CardCategory
 * @brief Enumeration for card categories
 */
enum class CardCategory {
    CREDIT,
    DEBIT,
    UNKNOWN
};

/**
 * @class CardToken
 * @brief Represents a tokenized payment card
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * card tokenization and secure storage of card information.
 */
class CardToken {
public:
    /**
     * @brief Constructor for a card token
     * @param cardNumber The card number to tokenize
     * @param cardholderName The cardholder's name
     * @param expiryDate The card expiry date (MM/YY)
     * @param customerId The customer ID
     * @param category The card category (credit or debit)
     */
    CardToken(const std::string& cardNumber, const std::string& cardholderName,
              const std::string& expiryDate, const std::string& customerId,
              CardCategory category = CardCategory::UNKNOWN);
    
    /**
     * @brief Get the token
     * @return The token
     */
    std::string getToken() const;
    
    /**
     * @brief Get the last four digits of the card number
     * @return The last four digits
     */
    std::string getLastFourDigits() const;
    
    /**
     * @brief Get the card type
     * @return The card type (e.g., Visa, Mastercard)
     */
    std::string getCardType() const;
    
    /**
     * @brief Get the expiry month
     * @return The expiry month
     */
    std::string getExpiryMonth() const;
    
    /**
     * @brief Get the expiry year
     * @return The expiry year
     */
    std::string getExpiryYear() const;
    
    /**
     * @brief Get the customer ID
     * @return The customer ID
     */
    std::string getCustomerId() const;
    
    /**
     * @brief Get the cardholder's name
     * @return The cardholder's name
     */
    std::string getCardholderName() const;
    
    /**
     * @brief Get a display name for the card
     * @return The display name (e.g., "Visa **** 1234")
     */
    std::string getDisplayName() const;
    
    /**
     * @brief Get the card category
     * @return The card category (credit or debit)
     */
    CardCategory getCardCategory() const;
    
    /**
     * @brief Set the card category
     * @param category The card category
     */
    void setCardCategory(CardCategory category);
    
protected:
    std::string m_token;
    std::string m_lastFourDigits;
    std::string m_cardType;
    std::string m_expiryMonth;
    std::string m_expiryYear;
    std::string m_customerId;
    std::string m_cardholderName;
    CardCategory m_cardCategory;
    
private:
    
    /**
     * @brief Generate a token for a card number
     * @param cardNumber The card number
     * @param customerId The customer ID
     * @return The generated token
     */
    static std::string generateToken(const std::string& cardNumber, const std::string& customerId);
    
    /**
     * @brief Detect the card type from the card number
     * @param cardNumber The card number
     * @return The card type
     */
    static std::string detectCardType(const std::string& cardNumber);
    
    /**
     * @brief Parse the expiry date
     * @param expiryDate The expiry date (MM/YY)
     * @param month Output parameter for the month
     * @param year Output parameter for the year
     * @return True if parsing was successful, false otherwise
     */
    static bool parseExpiryDate(const std::string& expiryDate, std::string& month, std::string& year);
};

/**
 * @class CardTokenFactory
 * @brief Factory for creating card tokens (Factory Method Pattern)
 */
class CardTokenFactory {
public:
    /**
     * @brief Create a new card token
     * @param cardNumber The card number to tokenize
     * @param cardholderName The cardholder's name
     * @param expiryDate The card expiry date (MM/YY)
     * @param customerId The customer ID
     * @param category The card category (credit or debit)
     * @return A unique pointer to the created card token
     */
    static std::unique_ptr<CardToken> createCardToken(
        const std::string& cardNumber, const std::string& cardholderName,
        const std::string& expiryDate, const std::string& customerId,
        CardCategory category = CardCategory::UNKNOWN);
    
    /**
     * @brief Create a card token from database data
     * @param token The token
     * @param lastFourDigits The last four digits of the card number
     * @param cardType The card type
     * @param expiryMonth The expiry month
     * @param expiryYear The expiry year
     * @param customerId The customer ID
     * @param cardholderName The cardholder's name
     * @param category The card category (credit or debit)
     * @return A unique pointer to the created card token
     */
    static std::unique_ptr<CardToken> createCardTokenFromDatabase(
        const std::string& token, const std::string& lastFourDigits,
        const std::string& cardType, const std::string& expiryMonth,
        const std::string& expiryYear, const std::string& customerId,
        const std::string& cardholderName, CardCategory category = CardCategory::UNKNOWN);
};

/**
 * @class CardManager
 * @brief Manages card tokens (Singleton Pattern)
 */
class CardManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static CardManager& getInstance();
    
    /**
     * @brief Delete copy constructor
     */
    CardManager(const CardManager&) = delete;
    
    /**
     * @brief Delete assignment operator
     */
    CardManager& operator=(const CardManager&) = delete;
    
    /**
     * @brief Add a card token
     * @param cardToken The card token to add
     */
    void addCardToken(std::unique_ptr<CardToken> cardToken);
    
    /**
     * @brief Get all card tokens for a customer
     * @param customerId The customer ID
     * @return Vector of card tokens
     */
    std::vector<const CardToken*> getCardTokensForCustomer(const std::string& customerId) const;
    
    /**
     * @brief Get a card token by token
     * @param token The token
     * @return Pointer to the card token, or nullptr if not found
     */
    const CardToken* getCardToken(const std::string& token) const;
    
    /**
     * @brief Delete a card token
     * @param token The token
     * @return True if the card token was deleted, false otherwise
     */
    bool deleteCardToken(const std::string& token);
    
private:
    /**
     * @brief Private constructor for singleton
     */
    CardManager();
    
    /**
     * @brief Vector of card tokens
     */
    std::vector<std::unique_ptr<CardToken>> m_cardTokens;
};

#endif // CARDTOKEN_H
