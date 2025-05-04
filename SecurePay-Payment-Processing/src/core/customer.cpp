#include "customer.h"

Customer::Customer() : m_name(""), m_email(""), m_billingAddress("") {}

Customer::Customer(const std::string& name, const std::string& email, const std::string& billingAddress)
    : m_name(name), m_email(email), m_billingAddress(billingAddress) {}

std::string Customer::getName() const {
    return m_name;
}

std::string Customer::getEmail() const {
    return m_email;
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

void Customer::setBillingAddress(const std::string& billingAddress) {
    m_billingAddress = billingAddress;
}
