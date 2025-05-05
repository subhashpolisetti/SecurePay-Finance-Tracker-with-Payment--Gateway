#ifndef ENHANCEDFRAUDSYSTEM_H
#define ENHANCEDFRAUDSYSTEM_H

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <chrono>
#include "transaction.h"
#include "fraudsystem.h"
#include "fraudalert.h"

/**
 * @struct FraudRule
 * @brief Represents a rule for fraud detection
 */
struct FraudRule {
    std::string name;
    float weight;
    std::function<bool(const Transaction&)> checkFunction;
    std::string description;
};

/**
 * @class EnhancedFraudSystem
 * @brief Enhanced fraud detection system with weighted scoring (Singleton Pattern)
 * 
 * This class follows the Singleton Pattern to ensure a single instance
 * and provides enhanced fraud detection with weighted scoring.
 */
class EnhancedFraudSystem {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static EnhancedFraudSystem& getInstance();
    
    /**
     * @brief Delete copy constructor
     */
    EnhancedFraudSystem(const EnhancedFraudSystem&) = delete;
    
    /**
     * @brief Delete assignment operator
     */
    EnhancedFraudSystem& operator=(const EnhancedFraudSystem&) = delete;
    
    /**
     * @brief Evaluate a transaction for fraud risk
     * @param transaction The transaction to evaluate
     * @return The fraud risk level
     */
    FraudRiskLevel evaluateTransaction(const Transaction& transaction);
    
    /**
     * @brief Get the fraud score for a transaction
     * @param transaction The transaction to evaluate
     * @param triggeredRules Output parameter for triggered rules
     * @return The fraud score (0.0 to 1.0)
     */
    float getFraudScore(const Transaction& transaction, std::vector<std::string>& triggeredRules);
    
    /**
     * @brief Add a custom fraud rule
     * @param name The rule name
     * @param weight The rule weight
     * @param checkFunction The function to check the rule
     * @param description The rule description
     */
    void addRule(const std::string& name, float weight, 
                std::function<bool(const Transaction&)> checkFunction,
                const std::string& description);
    
    /**
     * @brief Remove a fraud rule
     * @param name The rule name
     * @return True if the rule was removed, false otherwise
     */
    bool removeRule(const std::string& name);
    
    /**
     * @brief Get all fraud rules
     * @return Vector of fraud rules
     */
    const std::vector<FraudRule>& getRules() const;
    
    /**
     * @brief Set the high risk threshold
     * @param threshold The threshold (0.0 to 1.0)
     */
    void setHighRiskThreshold(float threshold);
    
    /**
     * @brief Set the medium risk threshold
     * @param threshold The threshold (0.0 to 1.0)
     */
    void setMediumRiskThreshold(float threshold);
    
    /**
     * @brief Get the high risk threshold
     * @return The high risk threshold
     */
    float getHighRiskThreshold() const;
    
    /**
     * @brief Get the medium risk threshold
     * @return The medium risk threshold
     */
    float getMediumRiskThreshold() const;
    
private:
    /**
     * @brief Private constructor for singleton
     */
    EnhancedFraudSystem();
    
    /**
     * @brief Create a fraud alert
     * @param transaction The transaction
     * @param level The fraud risk level
     * @param triggeredRules The triggered rules
     * @param score The fraud score
     */
    void createFraudAlert(const Transaction& transaction, 
                         FraudRiskLevel level,
                         const std::vector<std::string>& triggeredRules,
                         float score);
    
    /**
     * @brief Check if the amount is suspicious
     * @param amount The transaction amount
     * @return True if suspicious, false otherwise
     */
    static bool isAmountSuspicious(double amount);
    
    /**
     * @brief Check if the location is suspicious
     * @param billingAddress The billing address
     * @return True if suspicious, false otherwise
     */
    static bool isLocationSuspicious(const std::string& billingAddress);
    
    /**
     * @brief Check if the payment method is suspicious
     * @param paymentMethod The payment method
     * @return True if suspicious, false otherwise
     */
    static bool isPaymentMethodSuspicious(const PaymentMethod& paymentMethod);
    
    /**
     * @brief Check if the transaction time is suspicious
     * @return True if suspicious, false otherwise
     */
    static bool isTimeSuspicious();
    
    std::vector<FraudRule> m_rules;
    float m_highRiskThreshold;
    float m_mediumRiskThreshold;
};

#endif // ENHANCEDFRAUDSYSTEM_H
