#include "addmerchantdialog.h"
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddMerchantDialog::AddMerchantDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Add Merchant");
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("Enter Merchant Information", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14pt;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QFormLayout* formLayout = new QFormLayout();
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Enter merchant name");
    
    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("Enter merchant email");
    
    // Simple email validation
    QRegularExpression emailRegex("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,}\\b", 
                                 QRegularExpression::CaseInsensitiveOption);
    m_emailEdit->setValidator(new QRegularExpressionValidator(emailRegex, this));
    
    m_addressEdit = new QLineEdit(this);
    m_addressEdit->setPlaceholderText("Enter merchant address");
    
    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Email:", m_emailEdit);
    formLayout->addRow("Address:", m_addressEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_okButton = new QPushButton("OK", this);
    m_okButton->setEnabled(false);
    m_cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AddMerchantDialog::validateInput);
    connect(m_emailEdit, &QLineEdit::textChanged, this, &AddMerchantDialog::validateInput);
    connect(m_addressEdit, &QLineEdit::textChanged, this, &AddMerchantDialog::validateInput);
    
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

Merchant AddMerchantDialog::getMerchant() const {
    return Merchant(
        m_nameEdit->text().toUtf8().constData(),
        m_emailEdit->text().toUtf8().constData(),
        m_addressEdit->text().toUtf8().constData()
    );
}

void AddMerchantDialog::validateInput() {
    bool isValid = !m_nameEdit->text().isEmpty() &&
                  !m_emailEdit->text().isEmpty() &&
                  !m_addressEdit->text().isEmpty();
    
    m_okButton->setEnabled(isValid);
}
