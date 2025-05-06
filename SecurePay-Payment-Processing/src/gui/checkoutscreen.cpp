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
    connect(m_payNowButton, &QPushButton::clicked,
            this, &CheckoutScreen::onPayNowClicked);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &CheckoutScreen::onCancelClicked);
    
    // Initialize with first product
    if (m_productComboBox->count() > 0) {
        onProductSelected(0);
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
    
    // Get the authenticated customer
    const Customer* authenticatedCustomer = m_appController->getAuthenticatedCustomer();
    QString customerName = authenticatedCustomer ? 
                          QString::fromUtf8(authenticatedCustomer->getName().c_str()) : 
                          "No customer logged in";
    
    QLabel* customerLabel = new QLabel(customerName, customerGroup);
    customerLabel->setStyleSheet("font-weight: bold;");
    
    m_shippingAddressEdit = new QTextEdit(customerGroup);
    m_shippingAddressEdit->setPlaceholderText("Enter shipping address");
    m_shippingAddressEdit->setMaximumHeight(80);
    
    // Pre-fill shipping address if customer is authenticated
    if (authenticatedCustomer) {
        m_shippingAddressEdit->setText(QString::fromUtf8(authenticatedCustomer->getBillingAddress().c_str()));
    }
    
    customerLayout->addRow("Customer:", customerLabel);
    customerLayout->addRow("Shipping Address:", m_shippingAddressEdit);
    
    mainLayout->addWidget(customerGroup);
    
    // Payment information note
    QLabel* paymentNoteLabel = new QLabel("Payment details will be collected in the next step.", this);
    paymentNoteLabel->setStyleSheet("font-style: italic; color: #666;");
    paymentNoteLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(paymentNoteLabel);
    
    // Initialize payment method and card details to nullptr
    m_paymentMethodComboBox = nullptr;
    m_cardDetailsGroup = nullptr;
    m_cardNumberEdit = nullptr;
    m_cardholderNameEdit = nullptr;
    m_expiryDateEdit = nullptr;
    m_cvvEdit = nullptr;
    m_saveCardCheckBox = nullptr;
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_cancelButton = new QPushButton("Cancel", this);
    m_payNowButton = new QPushButton("Proceed to Payment", this);
    m_payNowButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_payNowButton);
    
    mainLayout->addLayout(buttonLayout);
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
    // This method is now a no-op since we're using the authenticated customer directly
    // It's kept for compatibility with existing code
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
    // This method is now a no-op since we've removed the card details form
    // It's kept for compatibility with existing code
}

bool CheckoutScreen::validateForm() {
    // Validate product selection
    int productIndex = m_productComboBox->currentIndex();
    if (productIndex < 0 || productIndex >= m_products.size()) {
        QMessageBox::warning(this, "Validation Error", "Please select a product.");
        return false;
    }
    
    // Validate authenticated customer
    const Customer* authenticatedCustomer = m_appController->getAuthenticatedCustomer();
    if (!authenticatedCustomer) {
        QMessageBox::warning(this, "Validation Error", "No customer is logged in. Please log in first.");
        return false;
    }
    
    // Validate shipping address
    if (m_shippingAddressEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a shipping address.");
        m_shippingAddressEdit->setFocus();
        return false;
    }
    
    return true;
}

bool CheckoutScreen::processPayment() {
    // This method is now a no-op since payment processing is handled in the main window
    // It's kept for compatibility with existing code
    return true;
}

void CheckoutScreen::onProductSelected(int index) {
    updateProductPrice();
}

void CheckoutScreen::onCustomerSelected(int index) {
    // This method is now a no-op since we've removed the customer selection
    // It's kept for compatibility with existing code
}

void CheckoutScreen::onPaymentMethodSelected(int index) {
    // This method is now a no-op since we've removed the payment method selection
    // It's kept for compatibility with existing code
}

void CheckoutScreen::onPayNowClicked() {
    if (!validateForm()) {
        return;
    }
    
    // Get selected product
    int productIndex = m_productComboBox->currentIndex();
    if (productIndex < 0 || productIndex >= m_products.size()) {
        return;
    }
    
    // Get authenticated customer
    const Customer* authenticatedCustomer = m_appController->getAuthenticatedCustomer();
    if (!authenticatedCustomer) {
        QMessageBox::warning(this, "Validation Error", "No customer is logged in. Please log in first.");
        return;
    }
    
    // Populate the checkout payload with basic information
    m_checkoutPayload.customerId = authenticatedCustomer->getName();
    m_checkoutPayload.productName = m_productComboBox->currentText().toUtf8().constData();
    m_checkoutPayload.amount = m_products[productIndex].price;
    m_checkoutPayload.shippingAddress = m_shippingAddressEdit->toPlainText().toUtf8().constData();
    
    // Set default payment method type
    m_checkoutPayload.paymentMethodType = "Credit Card";
    
    // Clear payment details - these will be collected in the main payment window
    m_checkoutPayload.cardNumber = "";
    m_checkoutPayload.cardholderName = "";
    m_checkoutPayload.expiryDate = "";
    m_checkoutPayload.cvv = "";
    m_checkoutPayload.cardToken = "";
    
    // Show confirmation message
    QMessageBox::information(this, "Checkout Complete", 
        "Your checkout information has been collected.\n\n"
        "Product: " + m_productComboBox->currentText() + "\n"
        "Amount: " + m_productPriceLabel->text() + "\n"
        "Shipping Address: " + m_shippingAddressEdit->toPlainText() + "\n\n"
        "You will now be redirected to the payment screen."
    );
    
    accept(); // Close dialog with accept result
}

void CheckoutScreen::onCancelClicked() {
    reject(); // Close dialog with reject result
}
