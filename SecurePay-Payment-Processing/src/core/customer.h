#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

class Customer {
public:
    Customer();
    Customer(const std::string& name, const std::string& email, const std::string& billingAddress);
    
    
    std::string getName() const;
    std::string getEmail() const;
    std::string getBillingAddress() const;
    
  
    void setName(const std::string& name);
    void setEmail(const std::string& email);
    void setBillingAddress(const std::string& billingAddress);
    
private:
    std::string m_name;
    std::string m_email;
    std::string m_billingAddress;
};

#endif 
