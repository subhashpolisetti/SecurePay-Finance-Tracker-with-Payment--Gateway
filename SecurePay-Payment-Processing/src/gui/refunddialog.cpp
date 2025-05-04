#include "refunddialog.h"
#include <QMessageBox>
#include <QButtonGroup>
#include <QGroupBox>

RefundDialog::RefundDialog(const std::vector<std::unique_ptr<Transaction>>& transactions,
                           RefundManager& refundManager,
                           QWidget* parent)
    : QDialog(parent), m_transactions(transactions), m_refundManager(refundManager) {
    
    setWindowTitle("Process Refund");
    setMinimumWidth(500);
    
    setupUI();
    
    // Populate transaction combo box
    for (const auto& transaction : m_transactions) {
        // Only show approved or partially refunded transactions
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            
            QString text = QString("%1 - %2 - $%3")
                .arg(QString::fromStdString(transaction->getTransactionId()))
                .arg(QString::fromStdString(transaction->getCustomer().getName()))
                .arg(transaction->getAmount(), 0, 'f', 2);
            
            m_transactionComboBox->addItem(text);
        }
    }
    
    if (m_transactionComboBox->count() > 0) {
        m_transactionComboBox->setCurrentIndex(0);
        onTransactionSelected(0);
    } else {
        m_transactionDetailsLabel->setText("No refundable transactions available");
        m_fullRefundRadio->setEnabled(false);
        m_partialRefundRadio->setEnabled(false);
        m_amountEdit->setEnabled(false);
        m_reasonEdit->setEnabled(false);
        m_processButton->setEnabled(false);
    }
}

void RefundDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Transaction selection
    QGroupBox* transactionGroup = new QGroupBox("Select Transaction", this);
    QVBoxLayout* transactionLayout = new QVBoxLayout(transactionGroup);
    
    m_transactionComboBox = new QComboBox(transactionGroup);
    m_transactionDetailsLabel = new QLabel(transactionGroup);
    m_transactionDetailsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_transactionDetailsLabel->setMinimumHeight(80);
    
    transactionLayout->addWidget(m_transactionComboBox);
    transactionLayout->addWidget(m_transactionDetailsLabel);
    
    // Refund options
    QGroupBox* refundGroup = new QGroupBox("Refund Options", this);
    QVBoxLayout* refundLayout = new QVBoxLayout(refundGroup);
    
    QButtonGroup* refundTypeGroup = new QButtonGroup(refundGroup);
    m_fullRefundRadio = new QRadioButton("Full Refund", refundGroup);
    m_partialRefundRadio = new QRadioButton("Partial Refund", refundGroup);
    refundTypeGroup->addButton(m_fullRefundRadio);
    refundTypeGroup->addButton(m_partialRefundRadio);
    m_fullRefundRadio->setChecked(true);
    
    QHBoxLayout* refundTypeLayout = new QHBoxLayout();
    refundTypeLayout->addWidget(m_fullRefundRadio);
    refundTypeLayout->addWidget(m_partialRefundRadio);
    refundTypeLayout->addStretch();
    
    QFormLayout* amountLayout = new QFormLayout();
    m_amountEdit = new QLineEdit(refundGroup);
    m_amountEdit->setValidator(new QDoubleValidator(0.01, 1000000.00, 2, refundGroup));
    m_amountEdit->setEnabled(false); // Disabled for full refund
    amountLayout->addRow("Refund Amount ($):", m_amountEdit);
    
    QVBoxLayout* reasonLayout = new QVBoxLayout();
    reasonLayout->addWidget(new QLabel("Reason for Refund:"));
    m_reasonEdit = new QTextEdit(refundGroup);
    m_reasonEdit->setMaximumHeight(80);
    reasonLayout->addWidget(m_reasonEdit);
    
    refundLayout->addLayout(refundTypeLayout);
    refundLayout->addLayout(amountLayout);
    refundLayout->addLayout(reasonLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_processButton = new QPushButton("Process Refund", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_processButton);
    buttonLayout->addWidget(m_cancelButton);
    
    // Main layout
    mainLayout->addWidget(transactionGroup);
    mainLayout->addWidget(refundGroup);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_transactionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RefundDialog::onTransactionSelected);
    connect(m_fullRefundRadio, &QRadioButton::toggled,
            this, &RefundDialog::onRefundTypeSelected);
    connect(m_processButton, &QPushButton::clicked,
            this, &RefundDialog::processRefund);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
}

void RefundDialog::onTransactionSelected(int index) {
    if (index < 0 || index >= static_cast<int>(m_transactions.size())) {
        m_transactionDetailsLabel->setText("No transaction selected");
        return;
    }
    
    // Find the selected transaction
    int refundableIndex = 0;
    const Transaction* selectedTransaction = nullptr;
    
    for (const auto& transaction : m_transactions) {
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            
            if (refundableIndex == index) {
                selectedTransaction = transaction.get();
                break;
            }
            
            refundableIndex++;
        }
    }
    
    if (!selectedTransaction) {
        m_transactionDetailsLabel->setText("Transaction not found");
        return;
    }
    
    // Update transaction details
    QString details = QString("<b>ID:</b> %1<br>"
                             "<b>Customer:</b> %2<br>"
                             "<b>Date:</b> %3<br>"
                             "<b>Amount:</b> $%4<br>"
                             "<b>Remaining Amount:</b> $%5<br>"
                             "<b>Payment Method:</b> %6")
        .arg(QString::fromStdString(selectedTransaction->getTransactionId()))
        .arg(QString::fromStdString(selectedTransaction->getCustomer().getName()))
        .arg(QString::fromStdString(selectedTransaction->getTimestamp()))
        .arg(selectedTransaction->getAmount(), 0, 'f', 2)
        .arg(selectedTransaction->getRemainingAmount(), 0, 'f', 2)
        .arg(QString::fromStdString(selectedTransaction->getPaymentMethod().getType()));
    
    m_transactionDetailsLabel->setText(details);
    
    // Update refund amount
    updateRefundAmount();
}

void RefundDialog::onRefundTypeSelected(bool checked) {
    if (m_fullRefundRadio->isChecked()) {
        m_amountEdit->setEnabled(false);
        updateRefundAmount();
    } else {
        m_amountEdit->setEnabled(true);
    }
}

void RefundDialog::updateRefundAmount() {
    if (m_transactionComboBox->currentIndex() < 0) {
        return;
    }
    
    // Find the selected transaction
    int refundableIndex = 0;
    const Transaction* selectedTransaction = nullptr;
    
    for (const auto& transaction : m_transactions) {
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            
            if (refundableIndex == m_transactionComboBox->currentIndex()) {
                selectedTransaction = transaction.get();
                break;
            }
            
            refundableIndex++;
        }
    }
    
    if (!selectedTransaction) {
        return;
    }
    
    // Set the full refund amount
    if (m_fullRefundRadio->isChecked()) {
        m_amountEdit->setText(QString::number(selectedTransaction->getRemainingAmount(), 'f', 2));
    }
}

void RefundDialog::processRefund() {
    if (m_transactionComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, "Error", "No transaction selected");
        return;
    }
    
    // Find the selected transaction
    int refundableIndex = 0;
    Transaction* selectedTransaction = nullptr;
    
    for (auto& transaction : m_transactions) {
        if (transaction->getStatus() == TransactionStatus::APPROVED ||
            transaction->getStatus() == TransactionStatus::PARTIALLY_REFUNDED) {
            
            if (refundableIndex == m_transactionComboBox->currentIndex()) {
                selectedTransaction = transaction.get();
                break;
            }
            
            refundableIndex++;
        }
    }
    
    if (!selectedTransaction) {
        QMessageBox::warning(this, "Error", "Transaction not found");
        return;
    }
    
    // Get the refund amount
    double refundAmount = 0.0;
    if (m_fullRefundRadio->isChecked()) {
        refundAmount = selectedTransaction->getRemainingAmount();
    } else {
        bool ok;
        refundAmount = m_amountEdit->text().toDouble(&ok);
        if (!ok || refundAmount <= 0.0 || refundAmount > selectedTransaction->getRemainingAmount()) {
            QMessageBox::warning(this, "Error", "Invalid refund amount");
            return;
        }
    }
    
    // Get the refund reason
    std::string reason = m_reasonEdit->toPlainText().toUtf8().constData();
    if (reason.empty()) {
        QMessageBox::warning(this, "Error", "Please provide a reason for the refund");
        return;
    }
    
    // Process the refund
    bool success = false;
    if (m_fullRefundRadio->isChecked()) {
        success = m_refundManager.processFullRefund(*selectedTransaction, reason);
    } else {
        success = m_refundManager.processPartialRefund(*selectedTransaction, refundAmount, reason);
    }
    
    if (success) {
        QMessageBox::information(this, "Success", "Refund processed successfully");
        accept();
    } else {
        QMessageBox::critical(this, "Error", "Failed to process refund");
    }
}
