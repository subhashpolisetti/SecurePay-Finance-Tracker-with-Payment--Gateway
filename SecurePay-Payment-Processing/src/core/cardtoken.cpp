#include "cardtoken.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>

CardToken::CardToken(const std::string& cardNumber, const std::string& cardholderName,
                    const std::string& expiryDate, const std::string& customerId,
                    CardCategory category)
    : m_cardholderName(cardholderName), m_customerId(customerId), m_cardCategory(category) {
    
    // Extract last four digits
    if (cardNumber.length() >= 4) {
        m_lastFourDigits = cardNumber.substr(cardNumber.length() - 4);
    } else {
        m_lastFourDigits = cardNumber;
    }
    
    // Detect card type
    m_cardType = detectCardType(cardNumber);
    
    // Parse expiry date
    if (!parseExpiryDate(expiryDate, m_expiryMonth, m_expiryYear)) {
        std::cerr << "Invalid expiry date format: " << expiryDate << std::endl;
        m_expiryMonth = "00";
        m_expiryYear = "00";
    }
    
    // Generate token
    m_token = generateToken(cardNumber, customerId);
}

std::string CardToken::getToken() const {
    return m_token;
}

std::string CardToken::getLastFourDigits() const {
    return m_lastFourDigits;
}

std::string CardToken::getCardType() const {
    return m_cardType;
}

std::string CardToken::getExpiryMonth() const {
    return m_expiryMonth;
}

std::string CardToken::getExpiryYear() const {
    return m_expiryYear;
}

std::string CardToken::getCustomerId() const {
    return m_customerId;
}

std::string CardToken::getCardholderName() const {
    return m_cardholderName;
}

std::string CardToken::getDisplayName() const {
    std::string categoryStr;
    switch (m_cardCategory) {
        case CardCategory::CREDIT:
            categoryStr = "Credit";
            break;
        case CardCategory::DEBIT:
            categoryStr = "Debit";
            break;
        default:
            categoryStr = "";
            break;
    }
    
    if (!categoryStr.empty()) {
        return categoryStr + " " + m_cardType + " **** " + m_lastFourDigits;
    } else {
        return m_cardType + " **** " + m_lastFourDigits;
    }
}

CardCategory CardToken::getCardCategory() const {
    return m_cardCategory;
}

void CardToken::setCardCategory(CardCategory category) {
    m_cardCategory = category;
}

std::string CardToken::generateToken(const std::string& cardNumber, const std::string& customerId) {
    // In a real implementation, this would use a secure tokenization algorithm
    // For the prototype, we'll just generate a random token
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex_chars = "0123456789ABCDEF";
    
    std::stringstream ss;
    ss << "TOK-";
    
    // Add a hash of the card number and customer ID
    size_t hash = std::hash<std::string>{}(cardNumber + customerId);
    ss << std::hex << std::uppercase << hash << "-";
    
    // Add some random characters
    for (int i = 0; i < 8; ++i) {
        ss << hex_chars[dis(gen)];
    }
    
    return ss.str();
}

std::string CardToken::detectCardType(const std::string& cardNumber) {
    // Remove spaces and dashes
    std::string cleanNumber;
    for (char c : cardNumber) {
        if (c != ' ' && c != '-') {
            cleanNumber += c;
        }
    }
    
    // Check for common card types based on prefix and length
    if (cleanNumber.empty()) {
        return "Unknown";
    }
    
    // Visa: Starts with 4, length 16
    if (cleanNumber[0] == '4') {
        return "Visa";
    }
    
    // Mastercard: Starts with 51-55, length 16
    if (cleanNumber.length() > 1 && cleanNumber[0] == '5' && cleanNumber[1] >= '1' && cleanNumber[1] <= '5') {
        return "Mastercard";
    }
    
    // American Express: Starts with 34 or 37, length 15
    if (cleanNumber.length() > 1 && cleanNumber[0] == '3' && (cleanNumber[1] == '4' || cleanNumber[1] == '7')) {
        return "Amex";
    }
    
    // Discover: Starts with 6011, length 16
    if (cleanNumber.length() > 3 && cleanNumber.substr(0, 4) == "6011") {
        return "Discover";
    }
    
    return "Unknown";
}

bool CardToken::parseExpiryDate(const std::string& expiryDate, std::string& month, std::string& year) {
    // Expected format: MM/YY
    std::regex pattern("^(0[1-9]|1[0-2])/([0-9]{2})$");
    std::smatch matches;
    
    if (std::regex_match(expiryDate, matches, pattern)) {
        month = matches[1].str();
        year = matches[2].str();
        return true;
    }
    
    return false;
}

std::unique_ptr<CardToken> CardTokenFactory::createCardToken(
    const std::string& cardNumber, const std::string& cardholderName,
    const std::string& expiryDate, const std::string& customerId,
    CardCategory category) {
    return std::make_unique<CardToken>(cardNumber, cardholderName, expiryDate, customerId, category);
}

// This constructor is used when loading from the database
class DatabaseCardToken : public CardToken {
public:
    DatabaseCardToken(const std::string& token, const std::string& lastFourDigits,
                     const std::string& cardType, const std::string& expiryMonth,
                     const std::string& expiryYear, const std::string& customerId,
                     const std::string& cardholderName,
                     CardCategory category = CardCategory::UNKNOWN)
        : CardToken("0000000000000000", cardholderName, "01/00", customerId, category) {
        // Override the values with the ones from the database
        m_token = token;
        m_lastFourDigits = lastFourDigits;
        m_cardType = cardType;
        m_expiryMonth = expiryMonth;
        m_expiryYear = expiryYear;
        m_customerId = customerId;
        m_cardholderName = cardholderName;
    }
};

std::unique_ptr<CardToken> CardTokenFactory::createCardTokenFromDatabase(
    const std::string& token, const std::string& lastFourDigits,
    const std::string& cardType, const std::string& expiryMonth,
    const std::string& expiryYear, const std::string& customerId,
    const std::string& cardholderName, CardCategory category) {
    return std::make_unique<DatabaseCardToken>(
        token, lastFourDigits, cardType, expiryMonth, expiryYear, customerId, cardholderName, category);
}

CardManager& CardManager::getInstance() {
    static CardManager instance;
    return instance;
}

CardManager::CardManager() {
    std::cout << "CardManager initialized" << std::endl;
}

void CardManager::addCardToken(std::unique_ptr<CardToken> cardToken) {
    if (cardToken) {
        // Check if a card with the same token already exists
        for (const auto& existingToken : m_cardTokens) {
            if (existingToken->getToken() == cardToken->getToken()) {
                std::cout << "Card token already exists: " << cardToken->getToken() << std::endl;
                return;
            }
        }
        
        m_cardTokens.push_back(std::move(cardToken));
    }
}

std::vector<const CardToken*> CardManager::getCardTokensForCustomer(const std::string& customerId) const {
    std::vector<const CardToken*> result;
    
    for (const auto& cardToken : m_cardTokens) {
        if (cardToken->getCustomerId() == customerId) {
            result.push_back(cardToken.get());
        }
    }
    
    return result;
}

const CardToken* CardManager::getCardToken(const std::string& token) const {
    for (const auto& cardToken : m_cardTokens) {
        if (cardToken->getToken() == token) {
            return cardToken.get();
        }
    }
    
    return nullptr;
}

bool CardManager::deleteCardToken(const std::string& token) {
    for (auto it = m_cardTokens.begin(); it != m_cardTokens.end(); ++it) {
        if ((*it)->getToken() == token) {
            m_cardTokens.erase(it);
            return true;
        }
    }
    
    return false;
}
