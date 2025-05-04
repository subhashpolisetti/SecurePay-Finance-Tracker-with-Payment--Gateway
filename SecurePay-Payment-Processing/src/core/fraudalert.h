#ifndef FRAUDALERT_H
#define FRAUDALERT_H

#include <string>
#include <memory>
#include <chrono>
#include "transaction.h"
#include "fraudsystem.h"

/**
 * @class FraudAlert
 * @brief Represents a fraud alert for a suspicious transaction
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * fraud alert data representation and basic operations.
 */
class FraudAlert {
public:
    /**
     * @brief Constructor for a fraud alert
     * @param transaction Reference to the suspicious transaction
     * @param riskLevel The fraud risk level
     * @param description Description of the fraud alert
     */
    FraudAlert(const Transaction& transaction, FraudRiskLevel riskLevel, const std::string& description);
    
    /**
     * @brief Get the alert ID
     * @return The unique alert ID
     */
    std::string getAlertId() const;
    
    /**
     * @brief Get the transaction
     * @return Reference to the suspicious transaction
     */
    const Transaction& getTransaction() const;
    
    /**
     * @brief Get the risk level
     * @return The fraud risk level
     */
    FraudRiskLevel getRiskLevel() const;
    
    /**
     * @brief Get the alert description
     * @return The description of the fraud alert
     */
    std::string getDescription() const;
    
    /**
     * @brief Get the timestamp of when the alert was created
     * @return The timestamp as a string
     */
    std::string getTimestamp() const;
    
    /**
     * @brief Check if the alert has been reviewed
     * @return True if the alert has been reviewed, false otherwise
     */
    bool isReviewed() const;
    
    /**
     * @brief Mark the alert as reviewed
     * @param reviewed True to mark as reviewed, false otherwise
     */
    void setReviewed(bool reviewed);
    
private:
    std::string m_alertId;
    const Transaction& m_transaction;
    FraudRiskLevel m_riskLevel;
    std::string m_description;
    std::chrono::system_clock::time_point m_timestamp;
    bool m_reviewed;
    
    /**
     * @brief Generate a unique alert ID
     * @return A unique alert ID
     */
    static std::string generateAlertId();
};

/**
 * @class FraudAlertFactory
 * @brief Factory for creating fraud alerts (Factory Method Pattern)
 */
class FraudAlertFactory {
public:
    /**
     * @brief Create a new fraud alert
     * @param transaction The suspicious transaction
     * @param riskLevel The fraud risk level
     * @param description Description of the fraud alert
     * @return A unique pointer to the created fraud alert
     */
    static std::unique_ptr<FraudAlert> createFraudAlert(
        const Transaction& transaction, FraudRiskLevel riskLevel, const std::string& description);
};

#endif // FRAUDALERT_H
