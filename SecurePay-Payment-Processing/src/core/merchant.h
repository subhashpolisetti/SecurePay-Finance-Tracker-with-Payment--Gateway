#ifndef MERCHANT_H
#define MERCHANT_H

#include <string>

/**
 * @class Merchant
 * @brief Represents a merchant in the payment system
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * merchant data representation and basic operations.
 */
class Merchant {
public:
    /**
     * @brief Default constructor
     */
    Merchant();
    
    /**
     * @brief Parameterized constructor
     * @param name The merchant's name
     * @param email The merchant's email
     * @param businessAddress The merchant's business address
     */
    Merchant(const std::string& name, const std::string& email, const std::string& businessAddress);
    
    /**
     * @brief Get the merchant's name
     * @return The merchant's name
     */
    std::string getName() const;
    
    /**
     * @brief Get the merchant's email
     * @return The merchant's email
     */
    std::string getEmail() const;
    
    /**
     * @brief Get the merchant's business address
     * @return The merchant's business address
     */
    std::string getBusinessAddress() const;
    
    /**
     * @brief Set the merchant's name
     * @param name The new name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Set the merchant's email
     * @param email The new email
     */
    void setEmail(const std::string& email);
    
    /**
     * @brief Set the merchant's business address
     * @param businessAddress The new business address
     */
    void setBusinessAddress(const std::string& businessAddress);
    
private:
    std::string m_name;
    std::string m_email;
    std::string m_businessAddress;
};

#endif // MERCHANT_H
