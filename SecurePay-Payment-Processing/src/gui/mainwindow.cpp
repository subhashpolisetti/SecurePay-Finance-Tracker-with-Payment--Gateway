#include "mainwindow.h"
#include "addcustomerdialog.h"
#include "addmerchantdialog.h"
#include "refunddialog.h"
#include "exportreportdialog.h"
#include "addcarddialog.h"
#include "managecardsdialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QDateTime>
#include <QToolBar>
#include <functional>
#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_appController(AppController::getInstance()) {
    // Initialize core components
    m_refundManager = &RefundManager::getInstance();
    m_reportManager = &ReportManager::getInstance();
    
    // Setup UI
    setupUI();
    
    // Register for transaction updates
    m_appController.setTransactionUpdateCallback(
        std::bind(&MainWindow::onTransactionUpdated, this, std::placeholders::_1));
    
    // Populate customer combo box
    for (const auto& customer : m_appController.getCustomers()) {
        m_customerComboBox->addItem(QString::fromUtf8(customer.getName().c_str()));
    }
    
    // Populate merchant combo box
    for (const auto& merchant : m_appController.getMerchants()) {
        m_merchantComboBox->addItem(QString::fromUtf8(merchant.getName().c_str()));
    }
    
    // Select first customer if available
    if (m_customerComboBox->count() > 0) {
        onCustomerSelected(0);
    }
    
    // Select first merchant if available
    if (m_merchantComboBox->count() > 0) {
        onMerchantSelected(0);
    }
    
    // Update transaction history
    updateCustomerTransactionHistory();
    updateMerchantTransactionHistory();
    updateRefundHistory();
    updateFraudAlerts();
    
    setWindowTitle("SecurePay Payment Processing");
    setMinimumSize(1000, 700);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Set object names for debugging
    setObjectName("MainWindow");
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Create role selection toolbar
    QToolBar* roleToolbar = new QToolBar("Role Selection", this);
    roleToolbar->setObjectName("roleToolbar");
    roleToolbar->setMovable(false);
    roleToolbar->setFloatable(false);
    
    QLabel* roleLabel = new QLabel("Select Role: ");
    roleLabel->setObjectName("roleLabel");
    roleToolbar->addWidget(roleLabel);
    
    m_roleComboBox = new QComboBox(roleToolbar);
    m_roleComboBox->setObjectName("roleComboBox");
    m_roleComboBox->addItem("Customer", static_cast<int>(UserRole::CUSTOMER));
    m_roleComboBox->addItem("Merchant", static_cast<int>(UserRole::MERCHANT));
    roleToolbar->addWidget(m_roleComboBox);
    
    addToolBar(roleToolbar);
    
    // Create stacked widget for different role views
    m_mainStack = new QStackedWidget(centralWidget);
    m_mainStack->setObjectName("mainStack");
    m_mainStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Create customer view with scroll area
    QScrollArea* customerScrollArea = new QScrollArea(m_mainStack);
    customerScrollArea->setObjectName("customerScrollArea");
    customerScrollArea->setWidgetResizable(true);
    customerScrollArea->setFrameShape(QFrame::NoFrame);
    customerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    customerScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_customerView = new QWidget(customerScrollArea);
    m_customerView->setObjectName("customerView");
    customerScrollArea->setWidget(m_customerView);
    m_mainStack->addWidget(customerScrollArea);
    
    // Create merchant view with scroll area
    QScrollArea* merchantScrollArea = new QScrollArea(m_mainStack);
    merchantScrollArea->setObjectName("merchantScrollArea");
    merchantScrollArea->setWidgetResizable(true);
    merchantScrollArea->setFrameShape(QFrame::NoFrame);
    merchantScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    merchantScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_merchantView = new QWidget(merchantScrollArea);
    m_merchantView->setObjectName("merchantView");
    merchantScrollArea->setWidget(m_merchantView);
    m_mainStack->addWidget(merchantScrollArea);
    
    // Setup individual views
    setupCustomerView();
    setupMerchantView();
    
    // Add stacked widget to main layout
    mainLayout->addWidget(m_mainStack);
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Set status bar
    statusBar()->showMessage("Ready");
    
    // Connect role selection signal
    connect(m_roleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRoleChanged);
    
    // Set initial role to Customer
    m_roleComboBox->setCurrentIndex(0);
    onRoleChanged(0);
    
    // Set minimum size for the window
    setMinimumSize(1000, 700);
}

void MainWindow::setupCustomerView() {
    QVBoxLayout* mainLayout = new QVBoxLayout(m_customerView);
    
    // Customer information section
    QGroupBox* customerGroup = new QGroupBox("Customer Information", m_customerView);
    QVBoxLayout* customerLayout = new QVBoxLayout(customerGroup);
    
    // Authentication buttons
    QHBoxLayout* authLayout = new QHBoxLayout();
    m_loginButton = new QPushButton("Login", customerGroup);
    m_logoutButton = new QPushButton("Logout", customerGroup);
    m_logoutButton->setEnabled(false); // Initially disabled until user logs in
    authLayout->addWidget(m_loginButton);
    authLayout->addWidget(m_logoutButton);
    
    // Customer selection
    QHBoxLayout* customerSelectionLayout = new QHBoxLayout();
    m_customerComboBox = new QComboBox(customerGroup);
    m_addCustomerButton = new QPushButton("Add Customer", customerGroup);
    customerSelectionLayout->addWidget(new QLabel("Select Customer:"));
    customerSelectionLayout->addWidget(m_customerComboBox, 1);
    customerSelectionLayout->addWidget(m_addCustomerButton);
    
    m_customerDetailsLabel = new QLabel(customerGroup);
    m_customerDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_customerDetailsLabel->setMinimumHeight(60);
    
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
    m_depositGroup = new QGroupBox("Deposit Funds", m_customerView);
    QVBoxLayout* depositLayout = new QVBoxLayout(m_depositGroup);
    
    QFormLayout* depositFormLayout = new QFormLayout();
    
    m_depositMethodComboBox = new QComboBox(m_depositGroup);
    m_depositMethodComboBox->addItem("Credit Card");
    m_depositMethodComboBox->addItem("Debit Card");
    m_depositMethodComboBox->addItem("Digital Wallet");
    
    m_depositAmountEdit = new QLineEdit(m_depositGroup);
    m_depositAmountEdit->setValidator(new QDoubleValidator(0.01, 10000.00, 2, m_depositGroup));
    m_depositAmountEdit->setPlaceholderText("Enter deposit amount (e.g., 100.00)");
    
    depositFormLayout->addRow("Payment Method:", m_depositMethodComboBox);
    depositFormLayout->addRow("Amount ($):", m_depositAmountEdit);
    
    m_depositButton = new QPushButton("Deposit Funds", m_depositGroup);
    m_depositButton->setMinimumHeight(40);
    
    depositLayout->addLayout(depositFormLayout);
    depositLayout->addWidget(m_depositButton);
    
    // Payment information section
    QGroupBox* paymentGroup = new QGroupBox("Payment Information", m_customerView);
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
    
    // Card management section
    QGroupBox* cardManagementGroup = new QGroupBox("Card Management", paymentGroup);
    QVBoxLayout* cardManagementLayout = new QVBoxLayout(cardManagementGroup);
    
    QHBoxLayout* savedCardsLayout = new QHBoxLayout();
    m_savedCardsComboBox = new QComboBox(cardManagementGroup);
    m_savedCardsComboBox->addItem("+ Add New Card");
    m_manageCardsButton = new QPushButton("Manage Cards", cardManagementGroup);
    savedCardsLayout->addWidget(new QLabel("Saved Cards:"));
    savedCardsLayout->addWidget(m_savedCardsComboBox, 1);
    savedCardsLayout->addWidget(m_manageCardsButton);
    
    cardManagementLayout->addLayout(savedCardsLayout);
    
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
    QGroupBox* resultGroup = new QGroupBox("Transaction Result", m_customerView);
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
    
    m_resultLabel = new QLabel(resultGroup);
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setMinimumHeight(60);
    m_resultLabel->setStyleSheet("font-size: 16pt;");
    
    resultLayout->addWidget(m_resultLabel);
    
    // Transaction history section
    QGroupBox* historyGroup = new QGroupBox("Transaction History", m_customerView);
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);
    
    m_customerTransactionTable = new QTableWidget(0, 5, historyGroup);
    m_customerTransactionTable->setHorizontalHeaderLabels({"ID", "Customer", "Amount", "Payment Method", "Status"});
    m_customerTransactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_customerTransactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_customerTransactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    m_exportCustomerReportButton = new QPushButton("Export Report", historyGroup);
    
    historyLayout->addWidget(m_customerTransactionTable);
    historyLayout->addWidget(m_exportCustomerReportButton);
    
    // Layout for result and history
    QHBoxLayout* transactionLayout = new QHBoxLayout();
    transactionLayout->addWidget(resultGroup);
    transactionLayout->addWidget(historyGroup);
    
    // E-commerce shopping button
    QPushButton* openCheckoutButton = new QPushButton("E-commerce Shopping", m_customerView);
    openCheckoutButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; font-size: 14pt;");
    openCheckoutButton->setMinimumHeight(60);
    openCheckoutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(openCheckoutButton, &QPushButton::clicked, this, &MainWindow::onOpenCheckoutClicked);
    
    // Add all sections to main layout
    mainLayout->addWidget(customerGroup);
    mainLayout->addWidget(m_depositGroup);
    mainLayout->addWidget(paymentGroup);
    mainLayout->addLayout(transactionLayout);
    mainLayout->addWidget(openCheckoutButton);
    
    // Connect signals
    connect(m_customerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCustomerSelected);
    connect(m_addCustomerButton, &QPushButton::clicked,
            this, &MainWindow::onAddCustomerClicked);
    connect(m_paymentMethodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPaymentMethodSelected);
    connect(m_savedCardsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSavedCardSelected);
    connect(m_manageCardsButton, &QPushButton::clicked,
            this, &MainWindow::onManageCardsClicked);
    connect(m_submitButton, &QPushButton::clicked,
            this, &MainWindow::onSubmitClicked);
    connect(m_exportCustomerReportButton, &QPushButton::clicked,
            this, &MainWindow::onExportCustomerReportClicked);
    connect(m_depositButton, &QPushButton::clicked,
            this, &MainWindow::onDepositClicked);
    connect(m_checkBalanceButton, &QPushButton::clicked,
            this, &MainWindow::onCheckBalanceClicked);
    connect(m_loginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginClicked);
    connect(m_logoutButton, &QPushButton::clicked,
            this, &MainWindow::onLogoutClicked);
}

void MainWindow::setupMerchantView() {
    QVBoxLayout* mainLayout = new QVBoxLayout(m_merchantView);
    
    // Merchant information section
    QGroupBox* merchantGroup = new QGroupBox("Merchant Information", m_merchantView);
    QVBoxLayout* merchantLayout = new QVBoxLayout(merchantGroup);
    
    QHBoxLayout* merchantSelectionLayout = new QHBoxLayout();
    m_merchantComboBox = new QComboBox(merchantGroup);
    m_addMerchantButton = new QPushButton("Add Merchant", merchantGroup);
    merchantSelectionLayout->addWidget(new QLabel("Select Merchant:"));
    merchantSelectionLayout->addWidget(m_merchantComboBox, 1);
    merchantSelectionLayout->addWidget(m_addMerchantButton);
    
    m_merchantDetailsLabel = new QLabel(merchantGroup);
    m_merchantDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_merchantDetailsLabel->setMinimumHeight(60);
    
    merchantLayout->addLayout(merchantSelectionLayout);
    merchantLayout->addWidget(m_merchantDetailsLabel);
    
    // Merchant tabs section
    m_merchantTabs = new QTabWidget(m_merchantView);
    
    // Transactions tab
    QWidget* transactionsTab = new QWidget(m_merchantTabs);
    QVBoxLayout* transactionsLayout = new QVBoxLayout(transactionsTab);
    
    m_merchantTransactionTable = new QTableWidget(0, 6, transactionsTab);
    m_merchantTransactionTable->setHorizontalHeaderLabels({"ID", "Customer", "Merchant", "Amount", "Payment Method", "Status"});
    m_merchantTransactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_merchantTransactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_merchantTransactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    m_processRefundButton = new QPushButton("Process Refund", transactionsTab);
    
    transactionsLayout->addWidget(m_merchantTransactionTable);
    transactionsLayout->addWidget(m_processRefundButton);
    
    m_merchantTabs->addTab(transactionsTab, "Transactions");
    
    // Refunds tab
    QWidget* refundsTab = new QWidget(m_merchantTabs);
    QVBoxLayout* refundsLayout = new QVBoxLayout(refundsTab);
    
    m_refundTable = new QTableWidget(0, 5, refundsTab);
    m_refundTable->setHorizontalHeaderLabels({"ID", "Transaction ID", "Amount", "Reason", "Date"});
    m_refundTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_refundTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_refundTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    refundsLayout->addWidget(m_refundTable);
    
    m_merchantTabs->addTab(refundsTab, "Refunds");
    
    // Fraud alerts tab
    QWidget* fraudAlertsTab = new QWidget(m_merchantTabs);
    QVBoxLayout* fraudAlertsLayout = new QVBoxLayout(fraudAlertsTab);
    
    m_fraudAlertTable = new QTableWidget(0, 6, fraudAlertsTab);
    m_fraudAlertTable->setHorizontalHeaderLabels({"ID", "Transaction ID", "Risk Level", "Risk Score", "Description", "Date"});
    m_fraudAlertTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_fraudAlertTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fraudAlertTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    fraudAlertsLayout->addWidget(m_fraudAlertTable);
    
    m_merchantTabs->addTab(fraudAlertsTab, "Fraud Alerts");
    
    // Analytics section
    m_analyticsGroup = new QGroupBox("Transaction Analytics", m_merchantView);
    QGridLayout* analyticsLayout = new QGridLayout(m_analyticsGroup);
    
    m_totalTransactionsLabel = new QLabel("Total Transactions: 0", m_analyticsGroup);
    m_totalVolumeLabel = new QLabel("Total Volume: $0.00", m_analyticsGroup);
    m_avgTransactionLabel = new QLabel("Average Transaction: $0.00", m_analyticsGroup);
    m_fraudRateLabel = new QLabel("Fraud Rate: 0.00%", m_analyticsGroup);
    m_refundRateLabel = new QLabel("Refund Rate: 0.00%", m_analyticsGroup);
    
    analyticsLayout->addWidget(m_totalTransactionsLabel, 0, 0);
    analyticsLayout->addWidget(m_totalVolumeLabel, 0, 1);
    analyticsLayout->addWidget(m_avgTransactionLabel, 1, 0);
    analyticsLayout->addWidget(m_fraudRateLabel, 1, 1);
    analyticsLayout->addWidget(m_refundRateLabel, 2, 0);
    
    // Export button
    m_exportMerchantReportButton = new QPushButton("Export Report", m_merchantView);
    m_exportMerchantReportButton->setMinimumHeight(40);
    
    // Add all sections to main layout
    mainLayout->addWidget(merchantGroup);
    mainLayout->addWidget(m_analyticsGroup);
    mainLayout->addWidget(m_merchantTabs, 1);
    mainLayout->addWidget(m_exportMerchantReportButton);
    
    // Connect signals
    connect(m_merchantComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onMerchantSelected);
    connect(m_addMerchantButton, &QPushButton::clicked,
            this, &MainWindow::onAddMerchantClicked);
    connect(m_processRefundButton, &QPushButton::clicked,
            this, &MainWindow::onProcessRefundClicked);
    connect(m_exportMerchantReportButton, &QPushButton::clicked,
            this, &MainWindow::onExportMerchantReportClicked);
}

void MainWindow::onRoleChanged(int index) {
    UserRole role = static_cast<UserRole>(m_roleComboBox->itemData(index).toInt());
    
    switch (role) {
        case UserRole::CUSTOMER:
            m_mainStack->setCurrentIndex(0); // Customer view
            statusBar()->showMessage("Customer Mode");
            std::cout << "Switched to Customer Mode" << std::endl;
            
            // Update customer view
            updateCustomerDetails();
            updateBalanceDisplay();
            updateCustomerTransactionHistory();
            
            // Ensure the view is visible and properly sized
            m_customerView->show();
            m_customerView->raise();
            m_customerView->update();
            break;
            
        case UserRole::MERCHANT:
            m_mainStack->setCurrentIndex(1); // Merchant view
            statusBar()->showMessage("Merchant Mode");
            std::cout << "Switched to Merchant Mode" << std::endl;
            
            // Update merchant view
            updateMerchantDetails();
            updateMerchantTransactionHistory();
            updateRefundHistory();
            updateFraudAlerts();
            
            // Ensure the view is visible and properly sized
            m_merchantView->show();
            m_merchantView->raise();
            m_merchantView->update();
            break;
    }
    
    // Force layout update and repaint
    if (QLayout* layout = centralWidget()->layout()) {
        layout->invalidate();
        layout->activate();
    }
    
    // Update the UI
    update();
    
    // Update the entire window
    update();
    repaint();
}

void MainWindow::updateCustomerDetails() {
    int index = m_customerComboBox->currentIndex();
    if (index >= 0 && index < static_cast<int>(m_appController.getCustomers().size())) {
        const Customer& customer = m_appController.getCustomers()[index];
        QString details = QString("<b>Name:</b> %1<br><b>Email:</b> %2<br><b>Address:</b> %3")
            .arg(QString::fromUtf8(customer.getName().c_str()))
            .arg(QString::fromUtf8(customer.getEmail().c_str()))
            .arg(QString::fromUtf8(customer.getBillingAddress().c_str()));
        m_customerDetailsLabel->setText(details);
    } else {
        m_customerDetailsLabel->setText("No customer selected");
    }
}

void MainWindow::updateMerchantDetails() {
    int index = m_merchantComboBox->currentIndex();
    if (index >= 0 && index < static_cast<int>(m_appController.getMerchants().size())) {
        const Merchant& merchant = m_appController.getMerchants()[index];
        QString details = QString("<b>Name:</b> %1<br><b>Email:</b> %2<br><b>Address:</b> %3")
            .arg(QString::fromUtf8(merchant.getName().c_str()))
            .arg(QString::fromUtf8(merchant.getEmail().c_str()))
            .arg(QString::fromUtf8(merchant.getBusinessAddress().c_str()));
        m_merchantDetailsLabel->setText(details);
    } else {
        m_merchantDetailsLabel->setText("No merchant selected");
    }
}

void MainWindow::updatePaymentMethodFields() {
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
    if (QLayout* layout = centralWidget()->layout()) {
        layout->invalidate();
        layout->activate();
    }
    
    // Update the UI
    update();
    
    // Update the widget
    update();
    repaint();
}

void MainWindow::updateCustomerTransactionHistory() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    m_customerTransactionTable->setRowCount(0);
    
    for (const auto& transaction : transactions) {
        int row = m_customerTransactionTable->rowCount();
        m_customerTransactionTable->insertRow(row);
        
        m_customerTransactionTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(transaction->getTransactionId().c_str())));
        m_customerTransactionTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(transaction->getCustomer().getName().c_str())));
        m_customerTransactionTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(transaction->getAmount(), 0, 'f', 2)));
        m_customerTransactionTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(transaction->getPaymentMethod().getType().c_str())));
        m_customerTransactionTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(Transaction::statusToString(transaction->getStatus()).c_str())));
    }
    
    // Resize columns to content
    m_customerTransactionTable->resizeColumnsToContents();
    
    // Update the table
    m_customerTransactionTable->update();
    
    // Update the UI
    update();
}

void MainWindow::updateMerchantTransactionHistory() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    m_merchantTransactionTable->setRowCount(0);
    
    for (const auto& transaction : transactions) {
        int row = m_merchantTransactionTable->rowCount();
        m_merchantTransactionTable->insertRow(row);
        
        m_merchantTransactionTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(transaction->getTransactionId().c_str())));
        m_merchantTransactionTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(transaction->getCustomer().getName().c_str())));
        m_merchantTransactionTable->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(transaction->getMerchant().getName().c_str())));
        m_merchantTransactionTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(transaction->getAmount(), 0, 'f', 2)));
        m_merchantTransactionTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(transaction->getPaymentMethod().getType().c_str())));
        m_merchantTransactionTable->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(Transaction::statusToString(transaction->getStatus()).c_str())));
    }
    
    // Resize columns to content
    m_merchantTransactionTable->resizeColumnsToContents();
    
    // Update the table
    m_merchantTransactionTable->update();
    
    // Update the UI
    update();
}

void MainWindow::updateRefundHistory() {
    // In a real implementation, this would fetch refunds from the RefundManager
    // For now, we'll just clear the table
    m_refundTable->setRowCount(0);
}

void MainWindow::updateFraudAlerts() {
    // In a real implementation, this would fetch fraud alerts from the FraudSystem
    // For now, we'll just clear the table
    m_fraudAlertTable->setRowCount(0);
}

void MainWindow::onCustomerSelected(int index) {
    updateCustomerDetails();
    updateBalanceDisplay();
}

void MainWindow::onDepositClicked() {
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
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
    
    Customer& customer = m_appController.getCustomersMutable()[customerIndex];
    QString paymentMethodType = m_depositMethodComboBox->currentText();
    
    // Get current balance
    double currentBalance = customer.getBalance(paymentMethodType.toUtf8().constData());
    
    // Add deposit amount to current balance
    customer.setBalance(paymentMethodType.toUtf8().constData(), currentBalance + amount);
    
    // Update balance display
    updateBalanceDisplay();
    
    // Clear deposit amount
    m_depositAmountEdit->clear();
    
    // Show success message
    QMessageBox::information(this, "Deposit Successful", 
                            QString("Successfully deposited $%1 to %2.\nNew balance: $%3")
                            .arg(amount, 0, 'f', 2)
                            .arg(paymentMethodType)
                            .arg(customer.getBalance(paymentMethodType.toUtf8().constData()), 0, 'f', 2));
}

void MainWindow::onCheckBalanceClicked() {
    updateBalanceDisplay();
    
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[customerIndex];
    
    QString balanceInfo = "Current Balances:\n\n";
    
    const auto& balances = customer.getAllBalances();
    for (const auto& [method, balance] : balances) {
        balanceInfo += QString("%1: $%2\n")
                      .arg(QString::fromUtf8(method.c_str()))
                      .arg(balance, 0, 'f', 2);
    }
    
    QMessageBox::information(this, "Account Balances", balanceInfo);
}

void MainWindow::updateBalanceDisplay() {
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        m_balanceLabel->setText("No customer selected");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[customerIndex];
    
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

void MainWindow::onMerchantSelected(int index) {
    updateMerchantDetails();
    updateAnalytics();
}

void MainWindow::updateAnalytics() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    // Calculate analytics
    int totalTransactions = 0;
    double totalVolume = 0.0;
    int flaggedTransactions = 0;
    int refundedTransactions = 0;
    
    for (const auto& transaction : transactions) {
        // Only count transactions for the selected merchant
        int merchantIndex = m_merchantComboBox->currentIndex();
        if (merchantIndex >= 0 && merchantIndex < static_cast<int>(m_appController.getMerchants().size())) {
            const Merchant& selectedMerchant = m_appController.getMerchants()[merchantIndex];
            
            if (transaction->getMerchant().getName() == selectedMerchant.getName()) {
                totalTransactions++;
                totalVolume += transaction->getAmount();
                
                if (transaction->getStatus() == TransactionStatus::FLAGGED_FOR_REVIEW) {
                    flaggedTransactions++;
                }
                
                if (transaction->getRefundedAmount() > 0) {
                    refundedTransactions++;
                }
            }
        }
    }
    
    // Update labels
    m_totalTransactionsLabel->setText(QString("Total Transactions: %1").arg(totalTransactions));
    m_totalVolumeLabel->setText(QString("Total Volume: $%1").arg(totalVolume, 0, 'f', 2));
    
    double avgTransaction = (totalTransactions > 0) ? (totalVolume / totalTransactions) : 0.0;
    m_avgTransactionLabel->setText(QString("Average Transaction: $%1").arg(avgTransaction, 0, 'f', 2));
    
    double fraudRate = (totalTransactions > 0) ? (static_cast<double>(flaggedTransactions) / totalTransactions * 100.0) : 0.0;
    m_fraudRateLabel->setText(QString("Fraud Rate: %1%").arg(fraudRate, 0, 'f', 2));
    
    double refundRate = (totalTransactions > 0) ? (static_cast<double>(refundedTransactions) / totalTransactions * 100.0) : 0.0;
    m_refundRateLabel->setText(QString("Refund Rate: %1%").arg(refundRate, 0, 'f', 2));
}

void MainWindow::onAddCustomerClicked() {
    AddCustomerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Customer customer = dialog.getCustomer();
        m_appController.addCustomer(customer);
        
        m_customerComboBox->addItem(QString::fromUtf8(customer.getName().c_str()));
        
        m_customerComboBox->setCurrentIndex(m_customerComboBox->count() - 1);
    }
}

void MainWindow::onAddMerchantClicked() {
    AddMerchantDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Merchant merchant = dialog.getMerchant();
        m_appController.addMerchant(merchant);
        
        m_merchantComboBox->addItem(QString::fromUtf8(merchant.getName().c_str()));
        
        m_merchantComboBox->setCurrentIndex(m_merchantComboBox->count() - 1);
    }
}

void MainWindow::onPaymentMethodSelected(int index) {
    updatePaymentMethodFields();
}

void MainWindow::onSavedCardSelected(int index) {
    if (index == 0) {
        // "Add New Card" option selected
        m_cardNumberEdit->clear();
        m_cardholderNameEdit->clear();
        m_expiryDateEdit->clear();
        m_cvvEdit->clear();
        return;
    }
    
    // Get the selected card token
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[customerIndex];
    auto cardTokens = m_appController.getCardTokensForCustomer(customer.getName());
    
    if (index - 1 < static_cast<int>(cardTokens.size())) {
        const CardToken* cardToken = cardTokens[index - 1];
        
        // Fill in the card details
        m_cardNumberEdit->setText("**** **** **** " + QString::fromStdString(cardToken->getLastFourDigits()));
        m_cardholderNameEdit->setText(QString::fromStdString(cardToken->getCardholderName()));
        m_expiryDateEdit->setText(QString::fromStdString(cardToken->getExpiryMonth() + "/" + cardToken->getExpiryYear()));
        m_cvvEdit->clear(); // CVV is never stored
    }
}

void MainWindow::onManageCardsClicked() {
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Error", "Please select a customer first.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[customerIndex];
    
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
    }
}

void MainWindow::onSubmitClicked() {
    bool amountValid;
    double amount = m_amountEdit->text().toDouble(&amountValid);
    
    if (!amountValid || amount <= 0) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid amount.");
        m_amountEdit->setFocus();
        return;
    }
    
    int customerIndex = m_customerComboBox->currentIndex();
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    const Customer& customer = m_appController.getCustomers()[customerIndex];
    
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
        
        updateCustomerTransactionHistory();
        updateMerchantTransactionHistory();
        
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
        
        statusBar()->showMessage("Transaction processed: " + QString::fromUtf8(transactionId.c_str()));
    } else {
        QMessageBox::critical(this, "Error", "Failed to create transaction.");
    }
}

void MainWindow::onProcessRefundClicked() {
    // Get selected transaction
    int row = m_merchantTransactionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a transaction to refund.");
        return;
    }
    
    // Get transaction ID
    QString transactionId = m_merchantTransactionTable->item(row, 0)->text();
    
    // Find transaction in history
    const Transaction* selectedTransaction = nullptr;
    for (const auto& transaction : m_appController.getTransactionHistory()) {
        if (transaction->getTransactionId() == transactionId.toUtf8().constData()) {
            selectedTransaction = transaction.get();
            break;
        }
    }
    
    if (!selectedTransaction) {
        QMessageBox::warning(this, "Error", "Transaction not found.");
        return;
    }
    
    // Open refund dialog
    RefundDialog dialog(m_appController.getTransactionHistory(), *m_refundManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Update transaction history
        updateCustomerTransactionHistory();
        updateMerchantTransactionHistory();
        updateRefundHistory();
        
        // Update balance display after refund
        updateBalanceDisplay();
    }
}

void MainWindow::onExportCustomerReportClicked() {
    ExportReportDialog dialog(*m_reportManager, this);
    dialog.exec();
}

void MainWindow::onExportMerchantReportClicked() {
    ExportReportDialog dialog(*m_reportManager, this);
    dialog.exec();
}

void MainWindow::onTransactionUpdated(const Transaction& transaction) {
    updateCustomerTransactionHistory();
    updateMerchantTransactionHistory();
    updateBalanceDisplay();
    
    statusBar()->showMessage("Transaction updated: " + QString::fromUtf8(transaction.getTransactionId().c_str()));
}

void MainWindow::showLoginDialog() {
    LoginDialog dialog(&m_appController, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Get the authenticated customer
        const Customer* authenticatedCustomer = dialog.getAuthenticatedCustomer();
        if (authenticatedCustomer) {
            // Find the customer in the combo box
            for (int i = 0; i < m_customerComboBox->count(); ++i) {
                const Customer& customer = m_appController.getCustomers()[i];
                if (customer.getUserId() == authenticatedCustomer->getUserId()) {
                    m_customerComboBox->setCurrentIndex(i);
                    break;
                }
            }
            
            // Update authentication status
            updateAuthenticationStatus();
            
            // Show success message
            statusBar()->showMessage("Logged in as " + QString::fromUtf8(authenticatedCustomer->getName().c_str()));
        }
    }
}

void MainWindow::updateAuthenticationStatus() {
    int customerIndex = m_customerComboBox->currentIndex();
    bool isAuthenticated = (customerIndex >= 0 && customerIndex < static_cast<int>(m_appController.getCustomers().size()));
    
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
}

void MainWindow::onLoginClicked() {
    showLoginDialog();
}

void MainWindow::onLogoutClicked() {
    // Clear current customer selection
    m_customerComboBox->setCurrentIndex(-1);
    
    // Update authentication status
    updateAuthenticationStatus();
    
    // Show logout message
    statusBar()->showMessage("Logged out successfully");
}

void MainWindow::onOpenCheckoutClicked() {
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

void MainWindow::receiveCheckoutPayload(const CheckoutPayload& payload) {
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
    
    // Switch to customer view
    m_roleComboBox->setCurrentIndex(0); // Customer role
    
    // Focus on the submit button
    m_submitButton->setFocus();
}
