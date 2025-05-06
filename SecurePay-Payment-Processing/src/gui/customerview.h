#ifndef CUSTOMERVIEW_H
#define CUSTOMERVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <memory>
#include "../core/appcontroller.h"
#include "../core/transaction.h"
#include "../core/cardtoken.h"
#include "checkoutscreen.h"

/**
 * @class CustomerView
 * @brief Widget for the customer view of the application
 * 
 * This class encapsulates all the UI elements and functionality
 * related to the customer view of the application.
 */
class CustomerView : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param appController Reference to the application controller
     * @param parent Parent widget
     */
    CustomerView(AppController& appController, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~CustomerView();
    
    /**
     * @brief Update the customer details display
     */
    void updateCustomerDetails();
    
    /**
     * @brief Update the payment method fields based on the selected payment method
     */
    void updatePaymentMethodFields();
    
    /**
     * @brief Update the customer transaction history table
     */
    void updateTransactionHistory();
    
    /**
     * @brief Update the balance display
     */
    void updateBalanceDisplay();
    
    /**
     * @brief Update the authentication status
     */
    void updateAuthenticationStatus();
    
    /**
     * @brief Update the deposit card combo box with saved cards
     */
    void updateDepositCardComboBox();
    
    /**
     * @brief Receive checkout payload from CheckoutScreen
     * @param payload The checkout payload
     */
    void receiveCheckoutPayload(const CheckoutPayload& payload);
    
signals:
    /**
     * @brief Signal emitted when a transaction is updated
     * @param transaction The updated transaction
     */
    void transactionUpdated(const Transaction& transaction);
    
private slots:
    /**
     * @brief Handle customer selection change
     * @param index The selected index
     */
    void onCustomerSelected(int index);
    
    /**
     * @brief Handle add customer button click
     */
    void onAddCustomerClicked();
    
    /**
     * @brief Handle payment method selection change
     * @param index The selected index
     */
    void onPaymentMethodSelected(int index);
    
    /**
     * @brief Handle saved card selection change
     * @param index The selected index
     */
    void onSavedCardSelected(int index);
    
    /**
     * @brief Handle manage cards button click
     */
    void onManageCardsClicked();
    
    /**
     * @brief Handle submit button click
     */
    void onSubmitClicked();
    
    /**
     * @brief Handle export customer report button click
     */
    void onExportCustomerReportClicked();
    
    /**
     * @brief Handle deposit button click
     */
    void onDepositClicked();
    
    /**
     * @brief Handle check balance button click
     */
    void onCheckBalanceClicked();
    
    /**
     * @brief Handle login button click
     */
    void onLoginClicked();
    
    /**
     * @brief Handle logout button click
     */
    void onLogoutClicked();
    
    /**
     * @brief Handle open checkout button click
     */
    void onOpenCheckoutClicked();
    
private:
    /**
     * @brief Initialize the UI components
     */
    void initUI();
    
    /**
     * @brief Show the login dialog
     */
    void showLoginDialog();
    
    // Core components
    AppController& m_appController;
    
    // Customer information
    QLabel* m_customerNameLabel;
    QPushButton* m_loginButton;
    QPushButton* m_logoutButton;
    QLabel* m_customerDetailsLabel;
    QLabel* m_balanceLabel;
    QPushButton* m_checkBalanceButton;
    
    // Currently logged in customer
    int m_currentCustomerIndex;
    
    // Deposit functionality
    QGroupBox* m_depositGroup;
    QComboBox* m_depositMethodComboBox;
    QComboBox* m_depositCardComboBox;
    QLineEdit* m_depositAmountEdit;
    QPushButton* m_depositButton;
    QPushButton* m_manageDepositCardsButton;
    
    // Payment information
    QLineEdit* m_amountEdit;
    QComboBox* m_paymentMethodComboBox;
    
    // Card management
    QComboBox* m_savedCardsComboBox;
    QPushButton* m_manageCardsButton;
    QLineEdit* m_cardNumberEdit;
    QLineEdit* m_cardholderNameEdit;
    QLineEdit* m_expiryDateEdit;
    QLineEdit* m_cvvEdit;
    QLineEdit* m_walletIdEdit;
    QLineEdit* m_walletEmailEdit;
    QGroupBox* m_cardFieldsGroup;
    QGroupBox* m_walletFieldsGroup;
    QPushButton* m_submitButton;
    
    // Transaction result
    QLabel* m_resultLabel;
    
    // Transaction history
    QTableWidget* m_transactionTable;
    QPushButton* m_exportReportButton;
    
    // E-commerce checkout
    QPushButton* m_openCheckoutButton;
};

#endif // CUSTOMERVIEW_H
