#ifndef ADDCARDDIALOG_H
#define ADDCARDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include "../core/cardtoken.h"

/**
 * @class AddCardDialog
 * @brief Dialog for adding a new payment card
 * 
 * This class provides a dialog for adding a new payment card
 * with validation and secure handling of card information.
 */
class AddCardDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param customerId The customer ID
     * @param parent The parent widget
     */
    explicit AddCardDialog(const std::string& customerId, QWidget* parent = nullptr);
    
    /**
     * @brief Get the created card token
     * @return The card token
     */
    std::unique_ptr<CardToken> getCardToken();
    
private slots:
    /**
     * @brief Handle the OK button click
     */
    void onOkClicked();
    
    /**
     * @brief Handle the Cancel button click
     */
    void onCancelClicked();
    
    /**
     * @brief Handle card number input changes
     * @param text The new text
     */
    void onCardNumberChanged(const QString& text);
    
private:
    /**
     * @brief Initialize the UI
     */
    void initUI();
    
    /**
     * @brief Validate the form
     * @return True if the form is valid, false otherwise
     */
    bool validateForm();
    
    /**
     * @brief Format the card number for display
     * @param cardNumber The card number
     * @return The formatted card number
     */
    QString formatCardNumber(const QString& cardNumber);
    
    /**
     * @brief Detect the card type from the card number
     * @param cardNumber The card number
     * @return The card type
     */
    QString detectCardType(const QString& cardNumber);
    
    /**
     * @brief Update the card type label
     */
    void updateCardTypeLabel();
    
    QLineEdit* m_cardNumberEdit;
    QLineEdit* m_cardholderNameEdit;
    QLineEdit* m_expiryDateEdit;
    QLineEdit* m_cvvEdit;
    QLabel* m_cardTypeLabel;
    QComboBox* m_cardCategoryComboBox;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    std::string m_customerId;
    std::unique_ptr<CardToken> m_cardToken;
};

#endif // ADDCARDDIALOG_H
