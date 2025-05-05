#include "checkoutscreen.h"
#include <QRegularExpressionValidator>
#include <QDateTime>
#include <iostream>

CheckoutScreen::CheckoutScreen(AppController* appController, QWidget* parent)
    : QDialog(parent), m_appController(appController) {
    
    setWindowTitle("E-Commerce Checkout");
    setMinimumSize(600, 500);
    
    // Initialize product data
    initProductData();
    
    // Initialize UI
    initUI();
    
    // Connect signals and slots
    connect(m_productComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CheckoutScreen::onProductSelected);
    connect(m_customerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CheckoutScreen::onCustomerSelected);
    connect(m_paymentMethodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CheckoutScreen::onPaymentMethodSelected);
    connect(m_payNowButton, &QPushButton::clicked,
            this, &CheckoutScreen::onPayNowClicked);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &CheckoutScreen::onCancelClicked);
    
    // Initialize with first product
    if (m_productComboBox->count() > 0) {
        onProductSelected(0);
    }
    
    // Initialize with first customer
    if (m_customerComboBox->count() > 0) {
        onCustomerSelected(0);
    }
}

CheckoutScreen::~CheckoutScreen() {
}

void CheckoutScreen::initUI() {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel("E-Commerce Checkout", this);
    titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Product selection section
    QGroupBox* productGroup = new QGroupBox("Product Selection", this);
    QFormLayout* productLayout = new QFormLayout(productGroup);
    
    m_productComboBox = new QComboBox(productGroup);
    for (const auto& product : m_products) {
        m_productComboBox->addItem(product.name);
    }
    
    m_productPriceLabel = new QLabel(productGroup);
    m_productPriceLabel->setStyleSheet("font-weight: bold; color: #006400;");
    
    productLayout->addRow("Select Product:", m_productComboBox);
    productLayout->addRow("Price:", m_productPriceLabel);
    
    mainLayout->addWidget(productGroup);
    
    // Customer information section
    QGroupBox* customerGroup = new QGroupBox("Customer Information", this);
    QFormLayout* customerLayout = new QFormLayout(customerGroup);
    
    m_customerComboBox = new QComboBox(customerGroup);
    for (const auto& customer : m_appController->getCustomers()) {
        m_customerComboBox->addItem(QString::fromUtf8(customer.getName().c_str()));
    }
    
    m_emailEdit = new QLineEdit(customerGroup);
    m_emailEdit->setPlaceholderText("Enter email for receipt");
    
    m_shippingAddressEdit = new QTextEdit(customerGroup);
    m_shippingAddressEdit->setPlaceholderText("Enter shipping address");
    m_shippingAddressEdit->setMaximumHeight(80);
    
    customerLayout->addRow("Customer:", m_customerComboBox);
    customerLayout->addRow("Email:", m_emailEdit);
    customerLayout->addRow("Shipping Address:", m_shippingAddressEdit);
    
    mainLayout->addWidget(customerGroup);
    
    // Payment method section
    QGroupBox* paymentGroup = new QGroupBox("Payment Method", this);
    QVBoxLayout* paymentLayout = new QVBoxLayout(paymentGroup);
    
    m_paymentMethodComboBox = new QComboBox(paymentGroup);
    m_paymentMethodComboBox->addItem("+ Add New Card");
    
    paymentLayout->addWidget(new QLabel("Select Payment Method:"));
    paymentLayout->addWidget(m_paymentMethodComboBox);
    
    // Card details form
    m_cardDetailsGroup = new QGroupBox("Card Details", paymentGroup);
    QGridLayout* cardLayout = new QGridLayout(m_cardDetailsGroup);
    
    m_cardNumberEdit = new QLineEdit(m_cardDetailsGroup);
    m_cardNumberEdit->setPlaceholderText("Enter card number");
    m_cardNumberEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{13,19}"), m_cardDetailsGroup));
    
    m_cardholderNameEdit = new QLineEdit(m_cardDetailsGroup);
    m_cardholderNameEdit->setPlaceholderText("Enter cardholder name");
    
    m_expiryDateEdit = new QLineEdit(m_cardDetailsGroup);
    m_expiryDateEdit->setPlaceholderText("MM/YY");
    m_expiryDateEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("([0-9]{2}/[0-9]{2})|([0-9]{4})"), m_cardDetailsGroup));
    
    m_cvvEdit = new QLineEdit(m_cardDetailsGroup);
    m_cvvEdit->setPlaceholderText("CVV");
    m_cvvEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{3,4}"), m_cardDetailsGroup));
    m_cvvEdit->setMaxLength(4);
    m_cvvEdit->setEchoMode(QLineEdit::Password);
    
    m_saveCardCheckBox = new QCheckBox("Save card for future use", m_cardDetailsGroup);
    
    cardLayout->addWidget(new QLabel("Card Number:"), 0, 0);
    cardLayout->addWidget(m_cardNumberEdit, 0, 1, 1, 3);
    
    cardLayout->addWidget(new QLabel("Cardholder Name:"), 1, 0);
    cardLayout->addWidget(m_cardholderNameEdit, 1, 1);
    
    cardLayout->addWidget(new QLabel("Expiry Date:"), 1, 2);
    cardLayout->addWidget(m_expiryDateEdit, 1, 3);
    
    cardLayout->addWidget(new QLabel("CVV:"), 2, 0);
    cardLayout->addWidget(m_cvvEdit, 2, 1);
    
    cardLayout->addWidget(m_saveCardCheckBox, 3, 0, 1, 4);
    
    paymentLayout->addWidget(m_cardDetailsGroup);
    mainLayout->addWidget(paymentGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_cancelButton = new QPushButton("Cancel", this);
    m_payNowButton = new QPushButton("Pay Now", this);
    m_payNowButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_payNowButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Initially hide card details
    showCardDetailsForm(true);
    
    // Load saved cards for the first customer
    if (m_customerComboBox->count() > 0) {
        loadSavedCards();
    }
}

void CheckoutScreen::initProductData() {
    // Sample product data
    m_products.push_back({"T-Shirt - Basic", 29.99});
    m_products.push_back({"Jeans - Classic", 59.99});
    m_products.push_back({"Sneakers - Sport", 89.99});
    m_products.push_back({"Backpack - Travel", 49.99});
    m_products.push_back({"Watch - Smart", 199.99});
    m_products.push_back({"Headphones - Wireless", 129.99});
    m_products.push_back({"Laptop - Ultrabook", 999.99});
    m_products.push_back({"Phone - Smartphone", 799.99});
}

void CheckoutScreen::loadSavedCards() {
    // Clear existing items except "Add New Card"
    while (m_paymentMethodComboBox->count() > 1) {
        m_paymentMethodComboBox->removeItem(1);
    }
    
    // Get the selected customer
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController->getCustomers().size())) {
        return;
    }
    
    const Customer& customer = m_appController->getCustomers()[customerIndex];
    
    // Get saved cards for the customer
    auto cardTokens = m_appController->getCardTokensForCustomer(customer.getName());
    
    // Add saved cards to the combo box
    for (const auto* cardToken : cardTokens) {
        QString displayText = QString::fromStdString(cardToken->getDisplayName());
        m_paymentMethodComboBox->addItem(displayText);
    }
    
    // Set email from customer data
    m_emailEdit->setText(QString::fromStdString(customer.getEmail()));
    
    // Set shipping address from customer data
    m_shippingAddressEdit->setText(QString::fromStdString(customer.getBillingAddress()));
}

void CheckoutScreen::updateProductPrice() {
    int index = m_productComboBox->currentIndex();
    if (index >= 0 && index < m_products.size()) {
        m_productPriceLabel->setText(QString("$%1").arg(m_products[index].price, 0, 'f', 2));
    } else {
        m_productPriceLabel->setText("$0.00");
    }
}

void CheckoutScreen::showCardDetailsForm(bool show) {
    m_cardDetailsGroup->setVisible(show);
    
    // Adjust dialog size
    if (show) {
        m_cardDetailsGroup->setFixedHeight(150);
    } else {
        m_cardDetailsGroup->setFixedHeight(0);
    }
    
    // Clear card fields if showing the form
    if (show) {
        m_cardNumberEdit->clear();
        m_cardholderNameEdit->clear();
        m_expiryDateEdit->clear();
        m_cvvEdit->clear();
        m_saveCardCheckBox->setChecked(false);
    }
    
    // Update layout
    layout()->activate();
    adjustSize();
}

bool CheckoutScreen::validateForm() {
    // Validate product selection
    int productIndex = m_productComboBox->currentIndex();
    if (productIndex < 0 || productIndex >= m_products.size()) {
        QMessageBox::warning(this, "Validation Error", "Please select a product.");
        return false;
    }
    
    // Validate customer selection
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController->getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return false;
    }
    
    // Validate email
    if (m_emailEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter an email address.");
        m_emailEdit->setFocus();
        return false;
    }
    
    // Validate shipping address
    if (m_shippingAddressEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a shipping address.");
        m_shippingAddressEdit->setFocus();
        return false;
    }
    
    // Validate payment method
    int paymentMethodIndex = m_paymentMethodComboBox->currentIndex();
    if (paymentMethodIndex < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a payment method.");
        return false;
    }
    
    // If "Add New Card" is selected, validate card details
    if (paymentMethodIndex == 0) {
        if (m_cardNumberEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter a card number.");
            m_cardNumberEdit->setFocus();
            return false;
        }
        
        if (m_cardholderNameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the cardholder name.");
            m_cardholderNameEdit->setFocus();
            return false;
        }
        
        if (m_expiryDateEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the expiry date.");
            m_expiryDateEdit->setFocus();
            return false;
        }
        
        if (m_cvvEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the CVV.");
            m_cvvEdit->setFocus();
            return false;
        }
    }
    
    return true;
}

bool CheckoutScreen::processPayment() {
    // Get selected product
    int productIndex = m_productComboBox->currentIndex();
    if (productIndex < 0 || productIndex >= m_products.size()) {
        return false;
    }
    
    // Get selected customer
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController->getCustomers().size())) {
        return false;
    }
    
    const Customer& customer = m_appController->getCustomers()[customerIndex];
    
    // Get product price
    double amount = m_products[productIndex].price;
    
    // Get payment method details
    int paymentMethodIndex = m_paymentMethodComboBox->currentIndex();
    std::string paymentMethodType = "Credit Card"; // Default to credit card
    std::string details1, details2, details3, details4;
    
    if (paymentMethodIndex == 0) {
        // New card
        details1 = m_cardNumberEdit->text().toUtf8().constData();
        details2 = m_cardholderNameEdit->text().toUtf8().constData();
        details3 = m_expiryDateEdit->text().toUtf8().constData();
        details4 = m_cvvEdit->text().toUtf8().constData();
        
        // Save card if checkbox is checked
        if (m_saveCardCheckBox->isChecked()) {
            auto cardToken = CardTokenFactory::createCardToken(
                details1,
                details2,
                details3,
                customer.getName()
            );
            
            if (cardToken) {
                m_appController->addCardToken(std::move(cardToken));
            }
        }
    } else {
        // Saved card
        auto cardTokens = m_appController->getCardTokensForCustomer(customer.getName());
        if (paymentMethodIndex - 1 < static_cast<int>(cardTokens.size())) {
            const CardToken* cardToken = cardTokens[paymentMethodIndex - 1];
            
            // Use token as details1
            details1 = cardToken->getToken();
            details2 = cardToken->getCardholderName();
            details3 = cardToken->getExpiryMonth() + "/" + cardToken->getExpiryYear();
            details4 = m_cvvEdit->text().toUtf8().constData(); // CVV is still required
        } else {
            return false;
        }
    }
    
    // Create a dummy e-commerce merchant if not already present
    Merchant ecommerceMerchant("EcomMerchant", "ecom@example.com", "Online Store");
    bool merchantExists = false;
    
    for (const auto& merchant : m_appController->getMerchants()) {
        if (merchant.getName() == "EcomMerchant") {
            ecommerceMerchant = merchant;
            merchantExists = true;
            break;
        }
    }
    
    if (!merchantExists) {
        m_appController->addMerchant(ecommerceMerchant);
    }
    
    // Create transaction
    auto transaction = m_appController->createTransaction(
        customer,
        ecommerceMerchant,
        paymentMethodType,
        details1,
        details2,
        details3,
        details4,
        amount
    );
    
    if (!transaction) {
        return false;
    }
    
    // Store shipping address in transaction metadata (in a real implementation)
    // transaction->setMetadata("shipping_address", m_shippingAddressEdit->toPlainText().toUtf8().constData());
    
    // Process transaction
    m_appController->processTransaction(std::move(transaction));
    
    return true;
}

void CheckoutScreen::onProductSelected(int index) {
    updateProductPrice();
}

void CheckoutScreen::onCustomerSelected(int index) {
    loadSavedCards();
}

void CheckoutScreen::onPaymentMethodSelected(int index) {
    // Show card details form if "Add New Card" is selected
    showCardDetailsForm(index == 0);
    
    // If a saved card is selected, we still need the CVV
    if (index > 0) {
        m_cvvEdit->clear();
        m_cvvEdit->setFocus();
    }
}

void CheckoutScreen::onPayNowClicked() {
    if (!validateForm()) {
        return;
    }
    
    if (processPayment()) {
        QMessageBox::information(this, "Payment Successful", 
            "Your payment has been processed successfully.\n\n"
            "Product: " + m_productComboBox->currentText() + "\n"
            "Amount: " + m_productPriceLabel->text() + "\n"
            "Shipping Address: " + m_shippingAddressEdit->toPlainText() + "\n\n"
            "A receipt has been sent to your email."
        );
        
        accept(); // Close dialog with accept result
    } else {
        QMessageBox::critical(this, "Payment Failed", 
            "There was an error processing your payment.\n"
            "Please check your payment details and try again."
        );
    }
}

void CheckoutScreen::onCancelClicked() {
    reject(); // Close dialog with reject result
}
