#include "customer.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Customer::Customer() 
    : m_userId(generateUserId()), 
      m_name(""), 
      m_email(""), 
      m_username(""),
      m_pin(""),
      m_billingAddress("") {
    // Initialize default balances
    m_balances["Credit Card"] = 1000.0;  // Default $1000 credit limit
    m_balances["Debit Card"] = 500.0;    // Default $500 balance
    m_balances["Digital Wallet"] = 200.0; // Default $200 balance
}

Customer::Customer(const std::string& name, const std::string& email, 
                   const std::string& username, const std::string& pin, 
                   const std::string& billingAddress)
    : m_userId(generateUserId()),
      m_name(name), 
      m_email(email), 
      m_username(username),
      m_pin(pin),
      m_billingAddress(billingAddress) {
    // Initialize default balances
    m_balances["Credit Card"] = 1000.0;  // Default $1000 credit limit
    m_balances["Debit Card"] = 500.0;    // Default $500 balance
    m_balances["Digital Wallet"] = 200.0; // Default $200 balance
}

Customer::Customer(const std::string& name, const std::string& email, const std::string& billingAddress)
    : m_userId(generateUserId()),
      m_name(name), 
      m_email(email), 
      m_username(email), // Default username to email for backward compatibility
      m_pin("1234"),     // Default PIN for backward compatibility
      m_billingAddress(billingAddress) {
    // Initialize default balances
    m_balances["Credit Card"] = 1000.0;  // Default $1000 credit limit
    m_balances["Debit Card"] = 500.0;    // Default $500 balance
    m_balances["Digital Wallet"] = 200.0; // Default $200 balance
}

std::string Customer::generateUserId() const {
    // Generate a random UUID-like string for the user ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    
    for (int i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4"; // Version 4 UUID
    for (int i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis(gen);
    ss << (8 + (dis(gen) % 4)); // Variant bits
    for (int i = 0; i < 2; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (int i = 0; i < 12; i++) {
        ss << dis(gen);
    }
    
    return ss.str();
}

std::string Customer::getUserId() const {
    return m_userId;
}

std::string Customer::getName() const {
    return m_name;
}

std::string Customer::getEmail() const {
    return m_email;
}

std::string Customer::getUsername() const {
    return m_username;
}

std::string Customer::getBillingAddress() const {
    return m_billingAddress;
}

void Customer::setName(const std::string& name) {
    m_name = name;
}

void Customer::setEmail(const std::string& email) {
    m_email = email;
}

void Customer::setUsername(const std::string& username) {
    m_username = username;
}

void Customer::setPin(const std::string& pin) {
    m_pin = pin;
}

void Customer::setBillingAddress(const std::string& billingAddress) {
    m_billingAddress = billingAddress;
}

bool Customer::authenticate(const std::string& usernameOrEmail, const std::string& pin) const {
    // Check if the provided username/email and PIN match
    return (usernameOrEmail == m_username || usernameOrEmail == m_email) && pin == m_pin;
}

void Customer::setBalance(const std::string& method, double amount) {
    m_balances[method] = amount;
    std::cout << "Set balance for " << method << " to $" << amount << std::endl;
}

double Customer::getBalance(const std::string& method) const {
    auto it = m_balances.find(method);
    if (it != m_balances.end()) {
        return it->second;
    }
    return 0.0;
}

bool Customer::deduct(const std::string& method, double amount) {
    auto it = m_balances.find(method);
    if (it != m_balances.end()) {
        if (it->second >= amount) {
            it->second -= amount;
            std::cout << "Deducted $" << amount << " from " << method << ". New balance: $" << it->second << std::endl;
            return true;
        } else {
            std::cout << "Insufficient funds in " << method << ". Current balance: $" << it->second << ", Required: $" << amount << std::endl;
        }
    } else {
        std::cout << "Payment method " << method << " not found" << std::endl;
    }
    return false;
}

void Customer::refund(const std::string& method, double amount) {
    auto it = m_balances.find(method);
    if (it != m_balances.end()) {
        it->second += amount;
        std::cout << "Refunded $" << amount << " to " << method << ". New balance: $" << it->second << std::endl;
    } else {
        // If method doesn't exist, create it
        m_balances[method] = amount;
        std::cout << "Created new payment method " << method << " with balance $" << amount << std::endl;
    }
}

const std::map<std::string, double>& Customer::getAllBalances() const {
    return m_balances;
}
