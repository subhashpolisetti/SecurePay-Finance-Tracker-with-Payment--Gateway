#include "paymentmethod.h"
#include <iostream>

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
    std::string maskedNumber = "XXXX-XXXX-XXXX-" + m_cardNumber.substr(m_cardNumber.length() - 4);
    return maskedNumber + " (" + m_cardholderName + ")";
}

std::string CreditCard::getCVV() const {
    return m_cvv;
}

PaymentMethod* CreditCard::clone() const {
    return new CreditCard(m_cardNumber, m_cardholderName, m_expiryDate, m_cvv);
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
    std::string maskedNumber = "XXXX-XXXX-XXXX-" + m_cardNumber.substr(m_cardNumber.length() - 4);
    return maskedNumber + " (" + m_cardholderName + ")";
}

std::string DebitCard::getCVV() const {
    return m_cvv;
}

PaymentMethod* DebitCard::clone() const {
    return new DebitCard(m_cardNumber, m_cardholderName, m_expiryDate, m_cvv);
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

std::string DigitalWallet::getCVV() const {
    // Digital wallets don't have CVVs, so return an empty string
    return "";
}

PaymentMethod* DigitalWallet::clone() const {
    return new DigitalWallet(m_walletId, m_email);
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
