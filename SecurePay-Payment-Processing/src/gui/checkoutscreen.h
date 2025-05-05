#ifndef CHECKOUTSCREEN_H
#define CHECKOUTSCREEN_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <memory>
#include "../core/appcontroller.h"
#include "../core/customer.h"
#include "../core/merchant.h"
#include "../core/transaction.h"
#include "../core/cardtoken.h"

/**
 * @class CheckoutScreen
 * @brief Dialog for simulating an e-commerce checkout process
 * 
 * This class provides a UI for simulating an e-commerce checkout process,
 * allowing users to select products, enter shipping information, and
 * process payments using the SecurePay payment gateway.
 */
class CheckoutScreen : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param appController Pointer to the application controller
     * @param parent Parent widget
     */
    CheckoutScreen(AppController* appController, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~CheckoutScreen();
    
private:
    // Core components
    AppController* m_appController;
    
    // Product selection
    QComboBox* m_productComboBox;
    QLabel* m_productPriceLabel;
    
    // Customer information
    QComboBox* m_customerComboBox;
    QLineEdit* m_emailEdit;
    QTextEdit* m_shippingAddressEdit;
    
    // Payment method
    QComboBox* m_paymentMethodComboBox;
    
    // Card details
    QGroupBox* m_cardDetailsGroup;
    QLineEdit* m_cardNumberEdit;
    QLineEdit* m_cardholderNameEdit;
    QLineEdit* m_expiryDateEdit;
    QLineEdit* m_cvvEdit;
    QCheckBox* m_saveCardCheckBox;
    
    // Buttons
    QPushButton* m_payNowButton;
    QPushButton* m_cancelButton;
    
    // Product data
    struct Product {
        QString name;
        double price;
    };
    
    QVector<Product> m_products;
    
    /**
     * @brief Initialize the UI components
     */
    void initUI();
    
    /**
     * @brief Initialize the product data
     */
    void initProductData();
    
    /**
     * @brief Load saved cards for the selected customer
     */
    void loadSavedCards();
    
    /**
     * @brief Update the product price label
     */
    void updateProductPrice();
    
    /**
     * @brief Show or hide the card details form
     * @param show Whether to show the form
     */
    void showCardDetailsForm(bool show);
    
    /**
     * @brief Validate the form inputs
     * @return True if all inputs are valid, false otherwise
     */
    bool validateForm();
    
    /**
     * @brief Process the payment
     * @return True if payment was successful, false otherwise
     */
    bool processPayment();
    
private slots:
    /**
     * @brief Handle product selection change
     * @param index The selected index
     */
    void onProductSelected(int index);
    
    /**
     * @brief Handle customer selection change
     * @param index The selected index
     */
    void onCustomerSelected(int index);
    
    /**
     * @brief Handle payment method selection change
     * @param index The selected index
     */
    void onPaymentMethodSelected(int index);
    
    /**
     * @brief Handle Pay Now button click
     */
    void onPayNowClicked();
    
    /**
     * @brief Handle Cancel button click
     */
    void onCancelClicked();
};

#endif // CHECKOUTSCREEN_H
