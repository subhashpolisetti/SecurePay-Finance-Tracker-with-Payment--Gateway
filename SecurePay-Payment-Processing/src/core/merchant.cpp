#include "merchant.h"

Merchant::Merchant() : m_name(""), m_email(""), m_businessAddress("") {}

Merchant::Merchant(const std::string& name, const std::string& email, const std::string& businessAddress)
    : m_name(name), m_email(email), m_businessAddress(businessAddress) {}

std::string Merchant::getName() const {
    return m_name;
}

std::string Merchant::getEmail() const {
    return m_email;
}

std::string Merchant::getBusinessAddress() const {
    return m_businessAddress;
}

void Merchant::setName(const std::string& name) {
    m_name = name;
}

void Merchant::setEmail(const std::string& email) {
    m_email = email;
}

void Merchant::setBusinessAddress(const std::string& businessAddress) {
    m_businessAddress = businessAddress;
}
