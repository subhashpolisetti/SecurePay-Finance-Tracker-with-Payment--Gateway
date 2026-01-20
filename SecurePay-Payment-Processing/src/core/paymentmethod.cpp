#include "paymentmethod.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>

namespace {

// Show only the last four digits; never index past the start of a short number
std::string maskCardNumber(const std::string& cardNumber) {
    if (cardNumber.length() < 4) {
        return "XXXX-XXXX-XXXX-XXXX";
    }
    return "XXXX-XXXX-XXXX-" + cardNumber.substr(cardNumber.length() - 4);
}

bool isAllDigits(const std::string& value) {
    return !value.empty() && std::all_of(value.begin(), value.end(),
                                         [](unsigned char c) { return std::isdigit(c); });
}

} // namespace

bool passesLuhnCheck(const std::string& cardNumber) {
    if (cardNumber.length() < 13 || cardNumber.length() > 19) {
        return false;
    }
    
    int sum = 0;
    bool doubleDigit = false;
    for (auto it = cardNumber.rbegin(); it != cardNumber.rend(); ++it) {
        if (*it < '0' || *it > '9') {
            return false;
        }
        int digit = *it - '0';
        if (doubleDigit) {
            digit *= 2;
            if (digit > 9) {
                digit -= 9;
            }
        }
        sum += digit;
        doubleDigit = !doubleDigit;
    }
    
    return sum % 10 == 0;
}

CreditCard::CreditCard(const std::string& cardNumber, const std::string& cardholderName, 
                       const std::string& expiryDate, const std::string& cvv)
    : m_cardNumber(cardNumber), m_cardholderName(cardholderName), 
      m_expiryDate(expiryDate), m_cvv(cvv) {}

bool CreditCard::process(double amount) const {
    std::cout << "Processing credit card payment of $" << amount << std::endl;
    
    return amount < 10000.0;
}

std::string CreditCard::getType() const {
    return "Credit Card";
}

std::string CreditCard::getDetails() const {
    return maskCardNumber(m_cardNumber) + " (" + m_cardholderName + ")";
}

bool isExpiryDateValid(const std::string& expiryDate, int currentYear, int currentMonth) {
    std::string digits = expiryDate;
    if (digits.length() == 5 && digits[2] == '/') {
        digits.erase(2, 1);
    }
    if (digits.length() != 4 || !isAllDigits(digits)) {
        return false;
    }
    
    int month = std::stoi(digits.substr(0, 2));
    int year = 2000 + std::stoi(digits.substr(2, 2));
    if (month < 1 || month > 12) {
        return false;
    }
    
    return year > currentYear || (year == currentYear && month >= currentMonth);
}

bool isExpiryDateValid(const std::string& expiryDate) {
    std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    return isExpiryDateValid(expiryDate, local.tm_year + 1900, local.tm_mon + 1);
}

bool isCvvValid(const std::string& cvv) {
    return (cvv.length() == 3 || cvv.length() == 4) && isAllDigits(cvv);
}

bool CreditCard::isValid() const {
    return passesLuhnCheck(m_cardNumber) && !m_cardholderName.empty() &&
           isExpiryDateValid(m_expiryDate) && isCvvValid(m_cvv);
}

std::unique_ptr<PaymentMethod> CreditCard::clone() const {
    return std::make_unique<CreditCard>(*this);
}

DebitCard::DebitCard(const std::string& cardNumber, const std::string& cardholderName, 
                     const std::string& expiryDate, const std::string& cvv)
    : m_cardNumber(cardNumber), m_cardholderName(cardholderName), 
      m_expiryDate(expiryDate), m_cvv(cvv) {}

bool DebitCard::process(double amount) const {
    std::cout << "Processing debit card payment of $" << amount << std::endl;
    
    return amount < 5000.0;
}

std::string DebitCard::getType() const {
    return "Debit Card";
}

std::string DebitCard::getDetails() const {
    return maskCardNumber(m_cardNumber) + " (" + m_cardholderName + ")";
}

bool DebitCard::isValid() const {
    return passesLuhnCheck(m_cardNumber) && !m_cardholderName.empty() &&
           isExpiryDateValid(m_expiryDate) && isCvvValid(m_cvv);
}

std::unique_ptr<PaymentMethod> DebitCard::clone() const {
    return std::make_unique<DebitCard>(*this);
}

DigitalWallet::DigitalWallet(const std::string& walletId, const std::string& email)
    : m_walletId(walletId), m_email(email) {}

bool DigitalWallet::process(double amount) const {
    std::cout << "Processing digital wallet payment of $" << amount << std::endl;
    
    return amount < 2000.0;
}

std::string DigitalWallet::getType() const {
    return "Digital Wallet";
}

std::string DigitalWallet::getDetails() const {
    return m_walletId + " (" + m_email + ")";
}

bool DigitalWallet::isValid() const {
    auto at = m_email.find('@');
    return !m_walletId.empty() && at != std::string::npos && at > 0 && at + 1 < m_email.length();
}

std::unique_ptr<PaymentMethod> DigitalWallet::clone() const {
    return std::make_unique<DigitalWallet>(*this);
}

std::unique_ptr<PaymentMethod> PaymentMethodFactory::createCreditCard(
    const std::string& cardNumber, const std::string& cardholderName,
    const std::string& expiryDate, const std::string& cvv) {
    return std::make_unique<CreditCard>(cardNumber, cardholderName, expiryDate, cvv);
}

std::unique_ptr<PaymentMethod> PaymentMethodFactory::createDebitCard(
    const std::string& cardNumber, const std::string& cardholderName,
    const std::string& expiryDate, const std::string& cvv) {
    return std::make_unique<DebitCard>(cardNumber, cardholderName, expiryDate, cvv);
}

std::unique_ptr<PaymentMethod> PaymentMethodFactory::createDigitalWallet(
    const std::string& walletId, const std::string& email) {
    return std::make_unique<DigitalWallet>(walletId, email);
}
