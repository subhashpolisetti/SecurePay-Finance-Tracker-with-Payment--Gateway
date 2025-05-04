#include "addcustomerdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

AddCustomerDialog::AddCustomerDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Add New Customer");
    setMinimumWidth(400);
    
    m_nameEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_addressEdit = new QLineEdit(this);
    
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    
    connect(m_okButton, &QPushButton::clicked, this, &AddCustomerDialog::validateForm);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Email:", m_emailEdit);
    formLayout->addRow("Billing Address:", m_addressEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    
    m_nameEdit->setFocus();
}

Customer AddCustomerDialog::getCustomer() const {
    return Customer(
        m_nameEdit->text().toUtf8().constData(),
        m_emailEdit->text().toUtf8().constData(),
        m_addressEdit->text().toUtf8().constData()
    );
}

void AddCustomerDialog::validateForm() {
    if (m_nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a name.");
        m_nameEdit->setFocus();
        return;
    }
    
    QRegularExpression emailRegex("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b");
    if (!emailRegex.match(m_emailEdit->text()).hasMatch()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid email address.");
        m_emailEdit->setFocus();
        return;
    }
    
    if (m_addressEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a billing address.");
        m_addressEdit->setFocus();
        return;
    }
    
    accept();
}
