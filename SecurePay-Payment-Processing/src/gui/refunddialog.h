#ifndef REFUNDDIALOG_H
#define REFUNDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleValidator>
#include <QTextEdit>
#include <QRadioButton>
#include "../core/transaction.h"
#include "../core/refundmanager.h"

/**
 * @class RefundDialog
 * @brief Dialog for processing refunds
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * the UI for processing refunds.
 */
class RefundDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param transactions Vector of transactions to choose from
     * @param refundManager Reference to the refund manager
     * @param parent Parent widget
     */
    RefundDialog(const std::vector<std::unique_ptr<Transaction>>& transactions,
                 RefundManager& refundManager,
                 QWidget* parent = nullptr);
    
    /**
     * @brief Constructor with preselected transaction
     * @param transactions Vector of transactions to choose from
     * @param refundManager Reference to the refund manager
     * @param selectedTransactionId ID of the transaction to preselect
     * @param parent Parent widget
     */
    RefundDialog(const std::vector<std::unique_ptr<Transaction>>& transactions,
                 RefundManager& refundManager,
                 const std::string& selectedTransactionId,
                 QWidget* parent = nullptr);
    
private:
    /**
     * @brief Transaction selection combo box
     */
    QComboBox* m_transactionComboBox;
    
    /**
     * @brief Transaction details label
     */
    QLabel* m_transactionDetailsLabel;
    
    /**
     * @brief Full refund radio button
     */
    QRadioButton* m_fullRefundRadio;
    
    /**
     * @brief Partial refund radio button
     */
    QRadioButton* m_partialRefundRadio;
    
    /**
     * @brief Refund amount input field
     */
    QLineEdit* m_amountEdit;
    
    /**
     * @brief Refund reason input field
     */
    QTextEdit* m_reasonEdit;
    
    /**
     * @brief Process button
     */
    QPushButton* m_processButton;
    
    /**
     * @brief Cancel button
     */
    QPushButton* m_cancelButton;
    
    /**
     * @brief Vector of transactions to choose from
     */
    const std::vector<std::unique_ptr<Transaction>>& m_transactions;
    
    /**
     * @brief Reference to the refund manager
     */
    RefundManager& m_refundManager;
    
    /**
     * @brief Setup the UI
     */
    void setupUI();
    
    /**
     * @brief Update the transaction details
     */
    void updateTransactionDetails();
    
    /**
     * @brief Update the refund amount field
     */
    void updateRefundAmount();
    
private slots:
    /**
     * @brief Handle transaction selection
     * @param index The selected index
     */
    void onTransactionSelected(int index);
    
    /**
     * @brief Handle refund type selection
     * @param checked Whether the radio button is checked
     */
    void onRefundTypeSelected(bool checked);
    
    /**
     * @brief Process the refund
     */
    void processRefund();
};

#endif // REFUNDDIALOG_H
