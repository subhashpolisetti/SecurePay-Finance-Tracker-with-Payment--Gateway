#include "mainwindow.h"
#include "addcustomerdialog.h"
#include "addmerchantdialog.h"
#include "refunddialog.h"
#include "exportreportdialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QDateTime>
#include <QToolBar>
#include <functional>
#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // Initialize core components
    m_appController = std::make_unique<AppController>();
    m_refundManager = &RefundManager::getInstance();
    m_reportManager = &ReportManager::getInstance();
    
    // Setup UI
    setupUI();
    
    // Register for transaction updates
    m_appController->setTransactionUpdateCallback(
        std::bind(&MainWindow::onTransactionUpdated, this, std::placeholders::_1));
    
    // Populate customer combo box
    for (const auto& customer : m_appController->getCustomers()) {
        m_customerComboBox->addItem(QString::fromUtf8(customer.getName().c_str()));
    }
    
    // Populate merchant combo box
    for (const auto& merchant : m_appController->getMerchants()) {
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
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create role selection toolbar
    QToolBar* roleToolbar = new QToolBar("Role Selection", this);
    roleToolbar->setMovable(false);
    roleToolbar->addWidget(new QLabel("Select Role: "));
    
    m_roleComboBox = new QComboBox(roleToolbar);
    m_roleComboBox->addItem("Customer", static_cast<int>(UserRole::CUSTOMER));
    m_roleComboBox->addItem("Merchant", static_cast<int>(UserRole::MERCHANT));
    roleToolbar->addWidget(m_roleComboBox);
    
    addToolBar(roleToolbar);
    
    // Create stacked widget for different role views
    m_mainStack = new QStackedWidget(centralWidget);
    
    // Create customer view
    m_customerView = new QWidget(m_mainStack);
    m_mainStack->addWidget(m_customerView);
    
    // Create merchant view
    m_merchantView = new QWidget(m_mainStack);
    m_mainStack->addWidget(m_merchantView);
    
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
}

void MainWindow::setupCustomerView() {
    QVBoxLayout* mainLayout = new QVBoxLayout(m_customerView);
    
    // Customer information section
    QGroupBox* customerGroup = new QGroupBox("Customer Information", m_customerView);
    QVBoxLayout* customerLayout = new QVBoxLayout(customerGroup);
    
    QHBoxLayout* customerSelectionLayout = new QHBoxLayout();
    m_customerComboBox = new QComboBox(customerGroup);
    m_addCustomerButton = new QPushButton("Add Customer", customerGroup);
    customerSelectionLayout->addWidget(new QLabel("Select Customer:"));
    customerSelectionLayout->addWidget(m_customerComboBox, 1);
    customerSelectionLayout->addWidget(m_addCustomerButton);
    
    m_customerDetailsLabel = new QLabel(customerGroup);
    m_customerDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_customerDetailsLabel->setMinimumHeight(60);
    
    customerLayout->addLayout(customerSelectionLayout);
    customerLayout->addWidget(m_customerDetailsLabel);
    
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
    
    // Add all sections to main layout
    mainLayout->addWidget(customerGroup);
    mainLayout->addWidget(paymentGroup);
    mainLayout->addLayout(transactionLayout);
    
    // Connect signals
    connect(m_customerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCustomerSelected);
    connect(m_addCustomerButton, &QPushButton::clicked,
            this, &MainWindow::onAddCustomerClicked);
    connect(m_paymentMethodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPaymentMethodSelected);
    connect(m_submitButton, &QPushButton::clicked,
            this, &MainWindow::onSubmitClicked);
    connect(m_exportCustomerReportButton, &QPushButton::clicked,
            this, &MainWindow::onExportCustomerReportClicked);
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
    
    m_fraudAlertTable = new QTableWidget(0, 4, fraudAlertsTab);
    m_fraudAlertTable->setHorizontalHeaderLabels({"ID", "Transaction ID", "Risk Level", "Date"});
    m_fraudAlertTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_fraudAlertTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fraudAlertTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    fraudAlertsLayout->addWidget(m_fraudAlertTable);
    
    m_merchantTabs->addTab(fraudAlertsTab, "Fraud Alerts");
    
    // Export button
    m_exportMerchantReportButton = new QPushButton("Export Report", m_merchantView);
    m_exportMerchantReportButton->setMinimumHeight(40);
    
    // Add all sections to main layout
    mainLayout->addWidget(merchantGroup);
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
            m_mainStack->setCurrentWidget(m_customerView);
            statusBar()->showMessage("Customer Mode");
            break;
        case UserRole::MERCHANT:
            m_mainStack->setCurrentWidget(m_merchantView);
            statusBar()->showMessage("Merchant Mode");
            break;
    }
}

void MainWindow::updateCustomerDetails() {
    int index = m_customerComboBox->currentIndex();
    if (index >= 0 && index < static_cast<int>(m_appController->getCustomers().size())) {
        const Customer& customer = m_appController->getCustomers()[index];
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
    if (index >= 0 && index < static_cast<int>(m_appController->getMerchants().size())) {
        const Merchant& merchant = m_appController->getMerchants()[index];
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
    
    std::cout << "Selected payment method: " << paymentMethod.toUtf8().constData() << std::endl;
    
    if (paymentMethod == "Credit Card" || paymentMethod == "Debit Card") {
        std::cout << "Showing card fields, hiding wallet fields" << std::endl;
        
        m_cardFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_walletFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        
        m_cardFieldsGroup->setFixedHeight(150);
        m_walletFieldsGroup->setFixedHeight(0);
        
        m_cardFieldsGroup->setVisible(true);
        m_walletFieldsGroup->setVisible(false);
    } else if (paymentMethod == "Digital Wallet") {
        std::cout << "Hiding card fields, showing wallet fields" << std::endl;
        
        m_cardFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        m_walletFieldsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        m_cardFieldsGroup->setFixedHeight(0);
        m_walletFieldsGroup->setFixedHeight(100);
        
        m_cardFieldsGroup->setVisible(false);
        m_walletFieldsGroup->setVisible(true);
    }
    
    if (QLayout* layout = centralWidget()->layout()) {
        layout->invalidate();
        layout->activate();
    }
    
    update();
    updateGeometry();
    repaint();
}

void MainWindow::updateCustomerTransactionHistory() {
    const auto& transactions = m_appController->getTransactionHistory();
    
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
}

void MainWindow::updateMerchantTransactionHistory() {
    const auto& transactions = m_appController->getTransactionHistory();
    
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
}

void MainWindow::onMerchantSelected(int index) {
    updateMerchantDetails();
}

void MainWindow::onAddCustomerClicked() {
    AddCustomerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Customer customer = dialog.getCustomer();
        m_appController->addCustomer(customer);
        
        m_customerComboBox->addItem(QString::fromUtf8(customer.getName().c_str()));
        
        m_customerComboBox->setCurrentIndex(m_customerComboBox->count() - 1);
    }
}

void MainWindow::onAddMerchantClicked() {
    AddMerchantDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Merchant merchant = dialog.getMerchant();
        m_appController->addMerchant(merchant);
        
        m_merchantComboBox->addItem(QString::fromUtf8(merchant.getName().c_str()));
        
        m_merchantComboBox->setCurrentIndex(m_merchantComboBox->count() - 1);
    }
}

void MainWindow::onPaymentMethodSelected(int index) {
    updatePaymentMethodFields();
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
    if (customerIndex < 0 || customerIndex >= static_cast<int>(m_appController->getCustomers().size())) {
        QMessageBox::warning(this, "Validation Error", "Please select a customer.");
        return;
    }
    
    const Customer& customer = m_appController->getCustomers()[customerIndex];
    
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
    if (m_appController->getMerchants().empty()) {
        QMessageBox::warning(this, "Validation Error", "No merchants available.");
        return;
    }
    
    const Merchant& merchant = m_appController->getMerchants()[0];
    
    auto transaction = m_appController->createTransaction(
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
        
        m_appController->processTransaction(std::move(transaction));
        
        m_amountEdit->clear();
        m_cardNumberEdit->clear();
        m_cardholderNameEdit->clear();
        m_expiryDateEdit->clear();
        m_cvvEdit->clear();
        m_walletIdEdit->clear();
        m_walletEmailEdit->clear();
        
        updateCustomerTransactionHistory();
        updateMerchantTransactionHistory();
        
        const auto& transactions = m_appController->getTransactionHistory();
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
    for (const auto& transaction : m_appController->getTransactionHistory()) {
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
    RefundDialog dialog(m_appController->getTransactionHistory(), *m_refundManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Update transaction history
        updateCustomerTransactionHistory();
        updateMerchantTransactionHistory();
        updateRefundHistory();
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
    
    statusBar()->showMessage("Transaction updated: " + QString::fromUtf8(transaction.getTransactionId().c_str()));
}
