#include "merchantview.h"
#include "addmerchantdialog.h"
#include "refunddialog.h"
#include "exportreportdialog.h"
#include <QMessageBox>
#include <QHeaderView>
#include <iostream>

MerchantView::MerchantView(AppController& appController, QWidget* parent)
    : QWidget(parent), m_appController(appController) {
    
    // Initialize core components
    m_refundManager = &RefundManager::getInstance();
    m_reportManager = &ReportManager::getInstance();
    
    // Initialize UI
    initUI();
    
    // Update tables
    updateTransactionHistory();
    updateRefundHistory();
    updateFraudAlerts();
    updateAnalytics();
}

MerchantView::~MerchantView() {
}

void MerchantView::initUI() {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Merchant information section
    QGroupBox* merchantGroup = new QGroupBox("Merchant Information", this);
    QVBoxLayout* merchantLayout = new QVBoxLayout(merchantGroup);
    
    m_merchantDetailsLabel = new QLabel(merchantGroup);
    m_merchantDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_merchantDetailsLabel->setMinimumHeight(60);
    
    // Set fixed merchant details - using one of the sample merchants from AppController
    QString details = QString("<b>Name:</b> Acme Store<br><b>Email:</b> acme@example.com<br><b>Address:</b> 789 Market St, San Francisco, CA");
    m_merchantDetailsLabel->setText(details);
    
    merchantLayout->addWidget(m_merchantDetailsLabel);
    
    // Analytics section
    m_analyticsGroup = new QGroupBox("Transaction Analytics", this);
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
    
    // Merchant tabs section
    m_merchantTabs = new QTabWidget(this);
    
    // Transactions tab
    QWidget* transactionsTab = new QWidget(m_merchantTabs);
    QVBoxLayout* transactionsLayout = new QVBoxLayout(transactionsTab);
    
    m_transactionTable = new QTableWidget(0, 6, transactionsTab);
    m_transactionTable->setHorizontalHeaderLabels({"ID", "Customer", "Merchant", "Amount", "Payment Method", "Status"});
    m_transactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_transactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    m_processRefundButton = new QPushButton("Process Refund", transactionsTab);
    
    transactionsLayout->addWidget(m_transactionTable);
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
    
    // Export button
    m_exportReportButton = new QPushButton("Export Report", this);
    m_exportReportButton->setMinimumHeight(40);
    
    // Add all sections to main layout
    mainLayout->addWidget(merchantGroup);
    mainLayout->addWidget(m_analyticsGroup);
    mainLayout->addWidget(m_merchantTabs, 1);
    mainLayout->addWidget(m_exportReportButton);
    
    // Connect signals
    connect(m_processRefundButton, &QPushButton::clicked,
            this, &MerchantView::onProcessRefundClicked);
    connect(m_exportReportButton, &QPushButton::clicked,
            this, &MerchantView::onExportMerchantReportClicked);
}

// Merchant details are now fixed in initUI

void MerchantView::updateTransactionHistory() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    m_transactionTable->setRowCount(0);
    
    for (const auto& transaction : transactions) {
        int row = m_transactionTable->rowCount();
        m_transactionTable->insertRow(row);
        
        m_transactionTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(transaction->getTransactionId().c_str())));
        m_transactionTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(transaction->getCustomer().getName().c_str())));
        m_transactionTable->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(transaction->getMerchant().getName().c_str())));
        m_transactionTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(transaction->getAmount(), 0, 'f', 2)));
        m_transactionTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(transaction->getPaymentMethod().getType().c_str())));
        m_transactionTable->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(Transaction::statusToString(transaction->getStatus()).c_str())));
    }
    
    // Resize columns to content
    m_transactionTable->resizeColumnsToContents();
    
    // Update the table
    m_transactionTable->update();
    
    // Update the UI
    update();
}

void MerchantView::updateRefundHistory() {
    // In a real implementation, this would fetch refunds from the RefundManager
    // For now, we'll just clear the table
    m_refundTable->setRowCount(0);
}

void MerchantView::updateFraudAlerts() {
    // In a real implementation, this would fetch fraud alerts from the FraudSystem
    // For now, we'll just clear the table
    m_fraudAlertTable->setRowCount(0);
}

void MerchantView::updateAnalytics() {
    const auto& transactions = m_appController.getTransactionHistory();
    
    // Calculate analytics
    int totalTransactions = 0;
    double totalVolume = 0.0;
    int flaggedTransactions = 0;
    int refundedTransactions = 0;
    
    // Fixed merchant name - using one of the sample merchants from AppController
    std::string merchantName = "Acme Store";
    
    for (const auto& transaction : transactions) {
        // Only count transactions for our fixed merchant
        if (transaction->getMerchant().getName() == merchantName) {
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

void MerchantView::onTransactionUpdated(const Transaction& transaction) {
    updateTransactionHistory();
    updateRefundHistory();
    updateFraudAlerts();
    updateAnalytics();
}

// These methods are no longer needed since we have a fixed merchant

void MerchantView::onProcessRefundClicked() {
    // Get selected transaction
    int row = m_transactionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a transaction to refund.");
        return;
    }
    
    // Get transaction ID
    QString transactionId = m_transactionTable->item(row, 0)->text();
    
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
    
    // Make sure the RefundManager has access to the customers
    m_refundManager->setCustomers(&m_appController.getCustomersMutable());
    
    // Open refund dialog with the selected transaction
    RefundDialog dialog(m_appController.getTransactionHistory(), *m_refundManager, 
                        transactionId.toUtf8().constData(), this);
    if (dialog.exec() == QDialog::Accepted) {
        // Update transaction history
        updateTransactionHistory();
        updateRefundHistory();
        updateAnalytics();
        
        // Save all data to ensure customer balances are persisted
        m_appController.saveAllData();
        
        // Show confirmation message
        QMessageBox::information(this, "Refund Processed", 
            "Refund has been processed successfully. The amount has been added to the customer's wallet.");
    }
}

void MerchantView::onExportMerchantReportClicked() {
    ExportReportDialog dialog(*m_reportManager, this);
    dialog.exec();
}
