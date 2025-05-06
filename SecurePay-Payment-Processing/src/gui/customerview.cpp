#include "customerview.h"
#include "addcustomerdialog.h"
#include "managecardsdialog.h"
#include "exportreportdialog.h"
#include "logindialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QDateTime>
#include <QDate>
#include <iostream>

CustomerView::CustomerView(AppController& appController, QWidget* parent)
    : QWidget(parent), m_appController(appController), m_currentCustomerIndex(-1) {
    
    // Initialize UI
    initUI();
    
    // Update transaction history
    updateTransactionHistory();
}

CustomerView::~CustomerView() {
}

void CustomerView::initUI() {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Customer information section
    QGroupBox* customerGroup = new QGroupBox("Customer Information", this);
    QVBoxLayout* customerLayout = new QVBoxLayout(customerGroup);
    
    // Authentication buttons
    QHBoxLayout* authLayout = new QHBoxLayout();
    m_loginButton = new QPushButton("Login", customerGroup);
    m_logoutButton = new QPushButton("Logout", customerGroup);
    m_logoutButton->setEnabled(false); // Initially disabled until user logs in
    authLayout->addWidget(m_loginButton);
    authLayout->addWidget(m_logoutButton);
    
    // Customer label
    QHBoxLayout* customerSelectionLayout = new QHBoxLayout();
    customerSelectionLayout->addWidget(new QLabel("Customer:"));
    customerSelectionLayout->addStretch(1);
    
    m_customerDetailsLabel = new QLabel(customerGroup);
    m_customerDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_customerDetailsLabel->setMinimumHeight(80);
    m_customerDetailsLabel->setWordWrap(true);
    m_customerDetailsLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_customerDetailsLabel->setTextFormat(Qt::RichText);
    m_customerDetailsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_customerDetailsLabel->setMargin(10);
    
    // Balance display
    m_balanceLabel = new QLabel(customerGroup);
    m_balanceLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_balanceLabel->setMinimumHeight(40);
    m_balanceLabel->setStyleSheet("font-weight: bold; color: #006400;");
    
    // Check balance button
    m_checkBalanceButton = new QPushButton("Check Balance", customerGroup);
    
    customerLayout->addLayout(authLayout);
    customerLayout->addLayout(customerSelectionLayout);
    customerLayout->addWidget(m_customerDetailsLabel);
    customerLayout->addWidget(m_balanceLabel);
    customerLayout->addWidget(m_checkBalanceButton);
    
    // Deposit functionality
    m_depositGroup = new QGroupBox("Deposit Funds", this);
    QVBoxLayout* depositLayout = new QVBoxLayout(m_depositGroup);
    
    QFormLayout* depositFormLayout = new QFormLayout();
    
    m_depositMethodComboBox = new QComboBox(m_depositGroup);
    m_depositMethodComboBox->addItem("Credit Card");
    m_depositMethodComboBox->addItem("Debit Card");
    m_depositMethodComboBox->addItem("Digital Wallet");
    
    // Card selection for deposit
    QHBoxLayout* depositCardLayout = new QHBoxLayout();
    m_depositCardComboBox = new QComboBox(m_depositGroup);
    m_depositCardComboBox->addItem("+ Add New Card");
    m_manageDepositCardsButton = new QPushButton("Manage Cards", m_depositGroup);
    depositCardLayout->addWidget(m_depositCardComboBox, 1);
    depositCardLayout->addWidget(m_manageDepositCardsButton);
    
    m_depositAmountEdit = new QLineEdit(m_depositGroup);
    m_depositAmountEdit->setValidator(new QDoubleValidator(0.01, 10000.00, 2, m_depositGroup));
    m_depositAmountEdit->setPlaceholderText("Enter deposit amount (e.g., 100.00)");
    
    depositFormLayout->addRow("Payment Method:", m_depositMethodComboBox);
    depositFormLayout->addRow("Card:", depositCardLayout);
    depositFormLayout->addRow("Amount ($):", m_depositAmountEdit);
    
    m_depositButton = new QPushButton("Deposit Funds", m_depositGroup);
    m_depositButton->setMinimumHeight(40);
    
    depositLayout->addLayout(depositFormLayout);
    depositLayout->addWidget(m_depositButton);
    
    // Payment information section
    QGroupBox* paymentGroup = new QGroupBox("Payment Information", this);
    QVBoxLayout* paymentLayout = new QVBoxLayout(paymentGroup);
    
    QFormLayout* paymentFormLayout = new QFormLayout();
    m_amountEdit = new QLineEdit(paymentGroup);
    m_amountEdit->setValidator(new QDoubleValidator(0.01, 10000.00, 2, paymentGroup));
    m_amountEdit->setPlaceholderText("Enter amount (e.g., 100.00)");
    
    m_paymentMethodComboBox = new QComboBox(paymentGroup);
    m_paymentMethodComboBox->addItem("Credit Card");
    m_paymentMethodComboBox->addItem("Debit Card");
    m_paymentMethodComboBox->addItem("Digital Wallet");
    
    paymentFormLayout->addRow("Amount ($):", m_amountEdit);
    paymentFormLayout->addRow("Payment Method:", m_paymentMethodComboBox);
    
    // Saved cards section
    QHBoxLayout* savedCardsLayout = new QHBoxLayout();
    m_savedCardsComboBox = new QComboBox(paymentGroup);
    m_savedCardsComboBox->addItem("+ Add New Card");
    m_manageCardsButton = new QPushButton("Manage Cards", paymentGroup);
    savedCardsLayout->addWidget(new QLabel("Saved Cards:"));
    savedCardsLayout->addWidget(m_savedCardsComboBox, 1);
    savedCardsLayout->addWidget(m_manageCardsButton);
    
    paymentFormLayout->addRow("", savedCardsLayout);
    
    // Card details section
    m_cardFieldsGroup = new QGroupBox("Card Details", paymentGroup);
    QGridLayout* cardGridLayout = new QGridLayout(m_cardFieldsGroup);
    
    m_cardNumberEdit = new QLineEdit(m_cardFieldsGroup);
    m_cardNumberEdit->setPlaceholderText("Enter card number");
    m_cardNumberEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{13,19}"), m_cardFieldsGroup));
    
    m_cardholderNameEdit = new QLineEdit(m_cardFieldsGroup);
    m_cardholderNameEdit->setPlaceholderText("Enter cardholder name");
    
    m_expiryDateEdit = new QLineEdit(m_cardFieldsGroup);
    m_expiryDateEdit->setPlaceholderText("MM/YY");
    m_expiryDateEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("([0-9]{2}/[0-9]{2})|([0-9]{4})"), m_cardFieldsGroup));
    
    m_cvvEdit = new QLineEdit(m_cardFieldsGroup);
    m_cvvEdit->setPlaceholderText("CVV");
    m_cvvEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{3,4}"), m_cardFieldsGroup));
    m_cvvEdit->setMaxLength(4);
    
    cardGridLayout->addWidget(new QLabel("Card Number:"), 0, 0);
    cardGridLayout->addWidget(m_cardNumberEdit, 0, 1, 1, 3);
    
    cardGridLayout->addWidget(new QLabel("Cardholder Name:"), 1, 0);
    cardGridLayout->addWidget(m_cardholderNameEdit, 1, 1);
    
    cardGridLayout->addWidget(new QLabel("Expiry Date:"), 1, 2);
    cardGridLayout->addWidget(m_expiryDateEdit, 1, 3);
    
    cardGridLayout->addWidget(new QLabel("CVV:"), 2, 2);
    cardGridLayout->addWidget(m_cvvEdit, 2, 3);
    
    cardGridLayout->setColumnStretch(0, 0);
    cardGridLayout->setColumnStretch(1, 2);
    cardGridLayout->setColumnStretch(2, 0);
    cardGridLayout->setColumnStretch(3, 1);
    
    // Wallet details section
    m_walletFieldsGroup = new QGroupBox("Wallet Details", paymentGroup);
    QFormLayout* walletFormLayout = new QFormLayout(m_walletFieldsGroup);
    
    m_walletIdEdit = new QLineEdit(m_walletFieldsGroup);
    m_walletIdEdit->setPlaceholderText("Enter wallet ID");
    
    m_walletEmailEdit = new QLineEdit(m_walletFieldsGroup);
    m_walletEmailEdit->setPlaceholderText("Enter wallet email");
    
    walletFormLayout->addRow("Wallet ID:", m_walletIdEdit);
    walletFormLayout->addRow("Email:", m_walletEmailEdit);
    
    // Submit button
    m_submitButton = new QPushButton("Submit Payment", paymentGroup);
    m_submitButton->setMinimumHeight(40);
    
    // Payment fields container
    QWidget* paymentFieldsContainer = new QWidget(paymentGroup);
    QVBoxLayout* fieldsContainerLayout = new QVBoxLayout(paymentFieldsContainer);
    fieldsContainerLayout->setContentsMargins(0, 0, 0, 0);
    
    fieldsContainerLayout->addWidget(m_cardFieldsGroup);
    fieldsContainerLayout->addWidget(m_walletFieldsGroup);
    
    paymentLayout->addLayout(paymentFormLayout);
    paymentLayout->addWidget(paymentFieldsContainer);
    
    QSpacerItem* buttonSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    paymentLayout->addSpacerItem(buttonSpacer);
    
    paymentLayout->addWidget(m_submitButton);
    
    // Set initial visibility
    m_cardFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_walletFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    
    m_cardFieldsGroup->setFixedHeight(150);
    m_walletFieldsGroup->setFixedHeight(0);
    
    m_cardFieldsGroup->setVisible(true);
    m_walletFieldsGroup->setVisible(false);
    
    // Transaction result section
    QGroupBox* resultGroup = new QGroupBox("Transaction Result", this);
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
    
    m_resultLabel = new QLabel(resultGroup);
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setMinimumHeight(60);
    m_resultLabel->setStyleSheet("font-size: 16pt;");
    
    resultLayout->addWidget(m_resultLabel);
    
    // Transaction history section
    QGroupBox* historyGroup = new QGroupBox("Transaction History", this);
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);
    
    m_transactionTable = new QTableWidget(0, 5, historyGroup);
    m_transactionTable->setHorizontalHeaderLabels({"ID", "Customer", "Amount", "Payment Method", "Status"});
    m_transactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_transactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    m_exportReportButton = new QPushButton("Export Report", historyGroup);
    
    historyLayout->addWidget(m_transactionTable);
    historyLayout->addWidget(m_exportReportButton);
    
    // Layout for result and history
    QHBoxLayout* transactionLayout = new QHBoxLayout();
    transactionLayout->addWidget(resultGroup);
    transactionLayout->addWidget(historyGroup);
    
    // E-commerce shopping button
    m_openCheckoutButton = new QPushButton("E-commerce Shopping", this);
    m_openCheckoutButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; font-size: 14pt;");
    m_openCheckoutButton->setMinimumHeight(60);
    m_openCheckoutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // Add all sections to main layout
    mainLayout->addWidget(customerGroup);
    mainLayout->addWidget(m_depositGroup);
    mainLayout->addWidget(paymentGroup);
    mainLayout->addLayout(transactionLayout);
    mainLayout->addWidget(m_openCheckoutButton);
    
    // Connect signals
    connect(m_paymentMethodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerView::onPaymentMethodSelected);
    connect(m_savedCardsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerView::onSavedCardSelected);
    connect(m_manageCardsButton, &QPushButton::clicked,
            this, &CustomerView::onManageCardsClicked);
    connect(m_manageDepositCardsButton, &QPushButton::clicked,
            this, &CustomerView::onManageCardsClicked);
    connect(m_submitButton, &QPushButton::clicked,
            this, &CustomerView::onSubmitClicked);
    connect(m_exportReportButton, &QPushButton::clicked,
            this, &CustomerView::onExportCustomerReportClicked);
    connect(m_depositButton, &QPushButton::clicked,
            this, &CustomerView::onDepositClicked);
    connect(m_checkBalanceButton, &QPushButton::clicked,
            this, &CustomerView::onCheckBalanceClicked);
    connect(m_loginButton, &QPushButton::clicked,
            this, &CustomerView::onLoginClicked);
    connect(m_logoutButton, &QPushButton::clicked,
            this, &CustomerView::onLogoutClicked);
    connect(m_openCheckoutButton, &QPushButton::clicked,
            this, &CustomerView::onOpenCheckoutClicked);
}

void CustomerView::updateCustomerDetails() {
    if (m_currentCustomerIndex >= 0 && m_currentCustomerIndex < static_cast<int>(m_appController.getCustomers().size())) {
        const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
        
        // Create a table-like layout for better alignment and responsiveness
        QString details = QString(
            "<table width='100%' style='border-spacing: 10px;'>"
            "<tr><td width='25%'><b>User ID:</b></td><td>%1</td></tr>"
            "<tr><td><b>Name:</b></td><td>%2</td></tr>"
            "<tr><td><b>Email:</b></td><td>%3</td></tr>"
            "<tr><td><b>Address:</b></td><td>%4</td></tr>"
            "</table>"
        )
        .arg(QString::fromUtf8(customer.getUserId().c_str()))
        .arg(QString::fromUtf8(customer.getName().c_str()))
        .arg(QString::fromUtf8(customer.getEmail().c_str()))
        .arg(QString::fromUtf8(customer.getBillingAddress().c_str()));
        
        m_customerDetailsLabel->setText(details);
    } else {
        m_customerDetailsLabel->setText("No customer selected");
    }
}

void CustomerView::updatePaymentMethodFields() {
    QString paymentMethod = m_paymentMethodComboBox->currentText();
    
    if (paymentMethod == "Credit Card" || paymentMethod == "Debit Card") {
        m_cardFieldsGroup->setVisible(true);
        m_walletFieldsGroup->setVisible(false);
        
        m_cardFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_walletFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        
        m_cardFieldsGroup->setFixedHeight(150);
        m_walletFieldsGroup->setFixedHeight(0);
    } else if (paymentMethod == "Digital Wallet") {
        m_cardFieldsGroup->setVisible(false);
        m_walletFieldsGroup->setVisible(true);
        
        m_cardFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        m_walletFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        m_cardFieldsGroup->setFixedHeight(0);
        m_walletFieldsGroup->setFixedHeight(100);
    }
    
    // Update layouts
    if (QLayout* l = this->layout()) {
        l->invalidate();
        l->activate();
    }
    
    // Update the UI
    update();
    
    // Update the widget
    update();
    repaint();
}

void CustomerView::updateTransactionHistory() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    m_transactionTable->setRowCount(0);
    
    for (const auto& transaction : transactions) {
        int row = m_transactionTable->rowCount();
        m_transactionTable->insertRow(row);
        
        m_transactionTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(transaction->getTransactionId().c_str())));
        m_transactionTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(transaction->getCustomer().getName().c_str())));
        m_transactionTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(transaction->getAmount(), 0, 'f', 2)));
        m_transactionTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(transaction->getPaymentMethod().getType().c_str())));
        m_transactionTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(Transaction::statusToString(transaction->getStatus()).c_str())));
    }
    
    // Resize columns to content
    m_transactionTable->resizeColumnsToContents();
    
    // Update the table
    m_transactionTable->update();
    
    // Update the UI
    update();
}

void CustomerView::updateBalanceDisplay() {
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        m_balanceLabel->setText("No customer selected");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    
    QString balanceText = "Balances: ";
    
    const auto& balances = customer.getAllBalances();
    bool first = true;
    for (const auto& [method, balance] : balances) {
        if (!first) {
            balanceText += " | ";
        }
        balanceText += QString("%1: $%2")
                      .arg(QString::fromUtf8(method.c_str()))
                      .arg(balance, 0, 'f', 2);
        first = false;
    }
    
    m_balanceLabel->setText(balanceText);
    
    // Update the UI
    update();
}

void CustomerView::updateAuthenticationStatus() {
    bool isAuthenticated = (m_currentCustomerIndex >= 0 && m_currentCustomerIndex < static_cast<int>(m_appController.getCustomers().size()));
    
    // Update UI based on authentication status
    m_loginButton->setEnabled(!isAuthenticated);
    m_logoutButton->setEnabled(isAuthenticated);
    
    // Enable/disable customer-specific functionality
    m_depositGroup->setEnabled(isAuthenticated);
    m_checkBalanceButton->setEnabled(isAuthenticated);
    m_submitButton->setEnabled(isAuthenticated);
    
    // Update customer details
    updateCustomerDetails();
    updateBalanceDisplay();
    
    // Update saved cards in deposit section
    if (isAuthenticated) {
        updateDepositCardComboBox();
    }
}

void CustomerView::updateDepositCardComboBox() {
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    
    // Clear the combo box
    m_depositCardComboBox->clear();
    m_depositCardComboBox->addItem("+ Add New Card");
    
    // Add saved cards
    auto cardTokens = m_appController.getCardTokensForCustomer(customer.getName());
    for (const auto* cardToken : cardTokens) {
        QString displayText = QString::fromStdString(cardToken->getDisplayName());
        // Store the token string instead of the pointer
        m_depositCardComboBox->addItem(displayText, QString::fromStdString(cardToken->getToken()));
    }
    
    // Also update the saved cards combo box in the payment section
    m_savedCardsComboBox->clear();
    m_savedCardsComboBox->addItem("+ Add New Card");
    
    for (const auto* cardToken : cardTokens) {
        QString displayText = QString::fromStdString(cardToken->getDisplayName());
        // Store the token string instead of the pointer
        m_savedCardsComboBox->addItem(displayText, QString::fromStdString(cardToken->getToken()));
    }
}

void CustomerView::showLoginDialog() {
    LoginDialog dialog(&m_appController, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Get the authenticated customer
        const Customer* authenticatedCustomer = dialog.getAuthenticatedCustomer();
        if (authenticatedCustomer) {
            // Find the customer in the customers list
            for (size_t i = 0; i < m_appController.getCustomers().size(); ++i) {
                const Customer& customer = m_appController.getCustomers()[i];
                if (customer.getUserId() == authenticatedCustomer->getUserId()) {
                    // Set the current customer index
                    m_currentCustomerIndex = static_cast<int>(i);
                    
                    // Update customer details
                    updateCustomerDetails();
                    
                    // Update balance display
                    updateBalanceDisplay();
                    
                    // Update authentication status
                    updateAuthenticationStatus();
                    break;
                }
            }
        }
    }
}

void CustomerView::onCustomerSelected(int index) {
    updateCustomerDetails();
    updateBalanceDisplay();
}

void CustomerView::onAddCustomerClicked() {
    AddCustomerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Customer customer = dialog.getCustomer();
        m_appController.addCustomer(customer);
        
        // Set the current customer index to the newly added customer
        m_currentCustomerIndex = static_cast<int>(m_appController.getCustomers().size()) - 1;
        
        // Update customer details
        updateCustomerDetails();
        updateBalanceDisplay();
        updateAuthenticationStatus();
    }
}

void CustomerView::onPaymentMethodSelected(int index) {
    updatePaymentMethodFields();
}

void CustomerView::onSavedCardSelected(int index) {
    if (index == 0) {
        // "Add New Card" option selected
        m_cardNumberEdit->clear();
        m_cardholderNameEdit->clear();
        m_expiryDateEdit->clear();
        m_cvvEdit->clear();
        return;
    }
    
    // Get the selected card token
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    auto cardTokens = m_appController.getCardTokensForCustomer(customer.getName());
    
    if (index - 1 < static_cast<int>(cardTokens.size())) {
        // Get the token string from the combo box
        QString tokenString = m_savedCardsComboBox->itemData(index).toString();
        
        // Find the card token with this token string
        const CardToken* cardToken = nullptr;
        for (const auto* token : cardTokens) {
            if (QString::fromStdString(token->getToken()) == tokenString) {
                cardToken = token;
                break;
            }
        }
        
        if (cardToken) {
            // Fill in the card details
            m_cardNumberEdit->setText("**** **** **** " + QString::fromStdString(cardToken->getLastFourDigits()));
            m_cardholderNameEdit->setText(QString::fromStdString(cardToken->getCardholderName()));
            m_expiryDateEdit->setText(QString::fromStdString(cardToken->getExpiryMonth() + "/" + cardToken->getExpiryYear()));
            m_cvvEdit->clear(); // CVV is never stored
        }
    }
}

void CustomerView::onManageCardsClicked() {
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Error", "Please select a customer first.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    
    ManageCardsDialog dialog(customer.getName(), this);
    if (dialog.exec() == QDialog::Accepted) {
        // Refresh the saved cards combo box
        m_savedCardsComboBox->clear();
        m_savedCardsComboBox->addItem("+ Add New Card");
        
        auto cardTokens = m_appController.getCardTokensForCustomer(customer.getName());
        for (const auto* cardToken : cardTokens) {
            QString displayText = QString::fromStdString(cardToken->getDisplayName());
            m_savedCardsComboBox->addItem(displayText);
        }
        
        // Select "Add New Card" option
        m_savedCardsComboBox->setCurrentIndex(0);
        
        // Also update the deposit card combo box
        updateDepositCardComboBox();
    }
}

void CustomerView::onSubmitClicked() {
    bool amountValid;
    double amount = m_amountEdit->text().toDouble(&amountValid);
    
    if (!amountValid || amount <= 0) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid amount.");
        m_amountEdit->setFocus();
        return;
    }
    
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    
    QString paymentMethodType = m_paymentMethodComboBox->currentText();
    std::string details1, details2, details3, details4;
    
    if (paymentMethodType == "Credit Card" || paymentMethodType == "Debit Card") {
        if (m_cardNumberEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter a card number.");
            m_cardNumberEdit->setFocus();
            return;
        }
        
        if (m_cardholderNameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the cardholder name.");
            m_cardholderNameEdit->setFocus();
            return;
        }
        
        if (m_expiryDateEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the expiry date.");
            m_expiryDateEdit->setFocus();
            return;
        }
        
        if (m_cvvEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the CVV.");
            m_cvvEdit->setFocus();
            return;
        }
        
        details1 = m_cardNumberEdit->text().toUtf8().constData();
        details2 = m_cardholderNameEdit->text().toUtf8().constData();
        details3 = m_expiryDateEdit->text().toUtf8().constData();
        details4 = m_cvvEdit->text().toUtf8().constData();
    } else if (paymentMethodType == "Digital Wallet") {
        if (m_walletIdEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter a wallet ID.");
            m_walletIdEdit->setFocus();
            return;
        }
        
        if (m_walletEmailEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter the wallet email.");
            m_walletEmailEdit->setFocus();
            return;
        }
        
        details1 = m_walletIdEdit->text().toUtf8().constData();
        details2 = m_walletEmailEdit->text().toUtf8().constData();
    }
    
    // For now, just use the first merchant in the list
    if (m_appController.getMerchants().empty()) {
        QMessageBox::warning(this, "Validation Error", "No merchants available.");
        return;
    }
    
    const Merchant& merchant = m_appController.getMerchants()[0];
    
    auto transaction = m_appController.createTransaction(
        customer,
        merchant,
        paymentMethodType.toUtf8().constData(),
        details1,
        details2,
        details3,
        details4,
        amount
    );
    
    if (transaction) {
        std::string transactionId = transaction->getTransactionId();
        
        m_appController.processTransaction(std::move(transaction));
        
        m_amountEdit->clear();
        m_cardNumberEdit->clear();
        m_cardholderNameEdit->clear();
        m_expiryDateEdit->clear();
        m_cvvEdit->clear();
        m_walletIdEdit->clear();
        m_walletEmailEdit->clear();
        
        updateTransactionHistory();
        
        const auto& transactions = m_appController.getTransactionHistory();
        TransactionStatus status = TransactionStatus::PENDING;
        
        for (const auto& tx : transactions) {
            if (tx->getTransactionId() == transactionId) {
                status = tx->getStatus();
                break;
            }
        }
        QString resultText;
        QString resultStyle;
        
        switch (status) {
            case TransactionStatus::APPROVED:
                resultText = "Transaction Approved";
                resultStyle = "color: green; font-weight: bold;";
                break;
            case TransactionStatus::DECLINED:
                resultText = "Transaction Declined";
                resultStyle = "color: red; font-weight: bold;";
                break;
            case TransactionStatus::FLAGGED_FOR_REVIEW:
                resultText = "Transaction Flagged for Review";
                resultStyle = "color: orange; font-weight: bold;";
                break;
            default:
                resultText = "Transaction Status: " + QString::fromUtf8(Transaction::statusToString(status).c_str());
                resultStyle = "color: black;";
                break;
        }
        
        m_resultLabel->setText(resultText);
        m_resultLabel->setStyleSheet("font-size: 16pt; " + resultStyle);
        
        // Update balance display after transaction
        updateBalanceDisplay();
        
        // Emit transaction updated signal
        for (const auto& tx : transactions) {
            if (tx->getTransactionId() == transactionId) {
                emit transactionUpdated(*tx);
                break;
            }
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to create transaction.");
    }
}

void CustomerView::onExportCustomerReportClicked() {
    ExportReportDialog dialog(ReportManager::getInstance(), this);
    dialog.exec();
}

void CustomerView::onDepositClicked() {
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    bool amountValid;
    double amount = m_depositAmountEdit->text().toDouble(&amountValid);
    
    if (!amountValid || amount <= 0) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid amount.");
        m_depositAmountEdit->setFocus();
        return;
    }
    
    Customer& customer = m_appController.getCustomersMutable()[m_currentCustomerIndex];
    QString paymentMethodType = m_depositMethodComboBox->currentText();
    
    // Check if a card is selected
    int cardIndex = m_depositCardComboBox->currentIndex();
    QString cardInfo;
    
    if (cardIndex > 0) {
        // A saved card is selected
        QString tokenString = m_depositCardComboBox->itemData(cardIndex).toString();
        
        // Find the card token with this token string
        const CardToken* cardToken = nullptr;
        auto cardTokens = m_appController.getCardTokensForCustomer(customer.getName());
        for (const auto* token : cardTokens) {
            if (QString::fromStdString(token->getToken()) == tokenString) {
                cardToken = token;
                break;
            }
        }
        
        if (cardToken) {
            // Determine if this is a credit or debit card based on the card category
            CardCategory category = cardToken->getCardCategory();
            if (category == CardCategory::CREDIT && paymentMethodType != "Credit Card") {
                QMessageBox::warning(this, "Validation Error", 
                    "Selected card is a credit card but deposit method is set to " + paymentMethodType + 
                    ". Please select matching payment method.");
                return;
            } else if (category == CardCategory::DEBIT && paymentMethodType != "Debit Card") {
                QMessageBox::warning(this, "Validation Error", 
                    "Selected card is a debit card but deposit method is set to " + paymentMethodType + 
                    ". Please select matching payment method.");
                return;
            }
            
            cardInfo = QString::fromStdString(cardToken->getDisplayName());
        }
    } else if (paymentMethodType != "Digital Wallet") {
        // No card selected but trying to deposit to a card
        QMessageBox::warning(this, "Validation Error", 
            "Please select a card or add a new one for " + paymentMethodType + " deposits.");
        return;
    }
    
    // Get current balance
    double currentBalance = customer.getBalance(paymentMethodType.toUtf8().constData());
    
    // Add deposit amount to current balance
    customer.setBalance(paymentMethodType.toUtf8().constData(), currentBalance + amount);
    
    // Update balance display
    updateBalanceDisplay();
    
    // Clear deposit amount
    m_depositAmountEdit->clear();
    
    // Show success message
    QString successMessage;
    if (!cardInfo.isEmpty()) {
        successMessage = QString("Successfully deposited $%1 to %2 using %3.\nNew balance: $%4")
            .arg(amount, 0, 'f', 2)
            .arg(paymentMethodType)
            .arg(cardInfo)
            .arg(customer.getBalance(paymentMethodType.toUtf8().constData()), 0, 'f', 2);
    } else {
        successMessage = QString("Successfully deposited $%1 to %2.\nNew balance: $%3")
            .arg(amount, 0, 'f', 2)
            .arg(paymentMethodType)
            .arg(customer.getBalance(paymentMethodType.toUtf8().constData()), 0, 'f', 2);
    }
    
    QMessageBox::information(this, "Deposit Successful", successMessage);
}

void CustomerView::onCheckBalanceClicked() {
    updateBalanceDisplay();
    
    if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[m_currentCustomerIndex];
    
    QString balanceInfo = "Current Balances:\n\n";
    
    const auto& balances = customer.getAllBalances();
    for (const auto& [method, balance] : balances) {
        balanceInfo += QString("%1: $%2\n")
                      .arg(QString::fromUtf8(method.c_str()))
                      .arg(balance, 0, 'f', 2);
    }
    
    QMessageBox::information(this, "Account Balances", balanceInfo);
}

void CustomerView::onLoginClicked() {
    showLoginDialog();
}

void CustomerView::onLogoutClicked() {
    // Clear current customer selection
    m_currentCustomerIndex = -1;
    
    // Update authentication status
    updateAuthenticationStatus();
}

void CustomerView::onOpenCheckoutClicked() {
    // Create a checkout screen
    CheckoutScreen* checkoutScreen = new CheckoutScreen(&m_appController, this);
    
    // Show the checkout screen
    if (checkoutScreen->exec() == QDialog::Accepted) {
        // Get the checkout payload
        const CheckoutPayload& payload = checkoutScreen->getCheckoutPayload();
        receiveCheckoutPayload(payload);
    }
    
    // Clean up
    delete checkoutScreen;
}

void CustomerView::receiveCheckoutPayload(const CheckoutPayload& payload) {
    // Set the amount
    m_amountEdit->setText(QString::number(payload.amount, 'f', 2));
    
    // Select the payment method
    int paymentMethodIndex = 0; // Default to Credit Card
    if (payload.paymentMethodType == "Debit Card") {
        paymentMethodIndex = 1;
    } else if (payload.paymentMethodType == "Digital Wallet") {
        paymentMethodIndex = 2;
    }
    m_paymentMethodComboBox->setCurrentIndex(paymentMethodIndex);
    
    // Update payment method fields
    updatePaymentMethodFields();
    
    // Fill in the payment details
    if (payload.paymentMethodType == "Credit Card" || payload.paymentMethodType == "Debit Card") {
        m_cardNumberEdit->setText(QString::fromStdString(payload.cardNumber));
        m_cardholderNameEdit->setText(QString::fromStdString(payload.cardholderName));
        m_expiryDateEdit->setText(QString::fromStdString(payload.expiryDate));
        m_cvvEdit->setText(QString::fromStdString(payload.cvv));
    } else if (payload.paymentMethodType == "Digital Wallet") {
        // For digital wallet, we don't have specific fields in the payload
        // so we'll just use some placeholder values
        m_walletIdEdit->setText("wallet_" + QString::fromStdString(payload.customerId));
        m_walletEmailEdit->setText(QString::fromStdString(payload.customerId) + "@example.com");
    }
    
    // Focus on the submit button
    m_submitButton->setFocus();
}
