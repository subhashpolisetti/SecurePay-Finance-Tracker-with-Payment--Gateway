#include "addcarddialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddCardDialog::AddCardDialog(const std::string& customerId, QWidget* parent)
    : QDialog(parent), m_customerId(customerId), m_cardToken(nullptr) {
    
    setWindowTitle("Add New Card");
    setMinimumWidth(400);
    
    initUI();
    
    connect(m_okButton, &QPushButton::clicked, this, &AddCardDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddCardDialog::onCancelClicked);
    connect(m_cardNumberEdit, &QLineEdit::textChanged, this, &AddCardDialog::onCardNumberChanged);
}

std::unique_ptr<CardToken> AddCardDialog::getCardToken() {
    return std::move(m_cardToken);
}

void AddCardDialog::initUI() {
    // Create form layout
    QFormLayout* formLayout = new QFormLayout();
    
    // Card number field
    m_cardNumberEdit = new QLineEdit();
    m_cardNumberEdit->setPlaceholderText("1234 5678 9012 3456");
    
    // Only allow digits and spaces
    QRegularExpression cardRegex("[0-9 ]{0,19}");
    QRegularExpressionValidator* cardValidator = new QRegularExpressionValidator(cardRegex, this);
    m_cardNumberEdit->setValidator(cardValidator);
    
    formLayout->addRow("Card Number:", m_cardNumberEdit);
    
    // Card type label
    m_cardTypeLabel = new QLabel("Card Type: Unknown");
    formLayout->addRow("", m_cardTypeLabel);
    
    // Cardholder name field
    m_cardholderNameEdit = new QLineEdit();
    m_cardholderNameEdit->setPlaceholderText("John Doe");
    formLayout->addRow("Cardholder Name:", m_cardholderNameEdit);
    
    // Expiry date field
    m_expiryDateEdit = new QLineEdit();
    m_expiryDateEdit->setPlaceholderText("MM/YY");
    
    // Only allow MM/YY format
    QRegularExpression expiryRegex("(0[1-9]|1[0-2])/[0-9]{2}");
    QRegularExpressionValidator* expiryValidator = new QRegularExpressionValidator(expiryRegex, this);
    m_expiryDateEdit->setValidator(expiryValidator);
    
    formLayout->addRow("Expiry Date:", m_expiryDateEdit);
    
    // CVV field
    m_cvvEdit = new QLineEdit();
    m_cvvEdit->setPlaceholderText("123");
    m_cvvEdit->setMaxLength(4);
    m_cvvEdit->setEchoMode(QLineEdit::Password);
    
    // Only allow digits
    QRegularExpression cvvRegex("[0-9]{3,4}");
    QRegularExpressionValidator* cvvValidator = new QRegularExpressionValidator(cvvRegex, this);
    m_cvvEdit->setValidator(cvvValidator);
    
    formLayout->addRow("CVV:", m_cvvEdit);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_okButton = new QPushButton("Add Card");
    m_cancelButton = new QPushButton("Cancel");
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void AddCardDialog::onOkClicked() {
    if (!validateForm()) {
        return;
    }
    
    // Get the card details
    QString cardNumber = m_cardNumberEdit->text().remove(' ');
    QString cardholderName = m_cardholderNameEdit->text();
    QString expiryDate = m_expiryDateEdit->text();
    
    // Create a card token
    m_cardToken = CardTokenFactory::createCardToken(
        cardNumber.toUtf8().constData(),
        cardholderName.toUtf8().constData(),
        expiryDate.toUtf8().constData(),
        m_customerId);
    
    accept();
}

void AddCardDialog::onCancelClicked() {
    reject();
}

void AddCardDialog::onCardNumberChanged(const QString& text) {
    // Format the card number
    QString formattedNumber = formatCardNumber(text);
    
    // Only update if the formatting changed the text
    if (formattedNumber != text) {
        int cursorPos = m_cardNumberEdit->cursorPosition();
        m_cardNumberEdit->setText(formattedNumber);
        m_cardNumberEdit->setCursorPosition(cursorPos);
    }
    
    // Update the card type label
    updateCardTypeLabel();
}

bool AddCardDialog::validateForm() {
    // Check card number
    QString cardNumber = m_cardNumberEdit->text().remove(' ');
    if (cardNumber.length() < 13 || cardNumber.length() > 19) {
        QMessageBox::warning(this, "Invalid Card", "Please enter a valid card number.");
        return false;
    }
    
    // Check cardholder name
    if (m_cardholderNameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Invalid Name", "Please enter the cardholder name.");
        return false;
    }
    
    // Check expiry date
    if (!m_expiryDateEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Invalid Expiry Date", "Please enter a valid expiry date in MM/YY format.");
        return false;
    }
    
    // Check CVV
    if (!m_cvvEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Invalid CVV", "Please enter a valid CVV code.");
        return false;
    }
    
    return true;
}

QString AddCardDialog::formatCardNumber(const QString& cardNumber) {
    // Remove existing spaces
    QString cleanNumber = cardNumber;
    cleanNumber.remove(' ');
    
    // Add spaces every 4 digits
    QString formattedNumber;
    for (int i = 0; i < cleanNumber.length(); ++i) {
        if (i > 0 && i % 4 == 0) {
            formattedNumber += ' ';
        }
        formattedNumber += cleanNumber[i];
    }
    
    return formattedNumber;
}

QString AddCardDialog::detectCardType(const QString& cardNumber) {
    // Remove spaces
    QString cleanNumber = cardNumber;
    cleanNumber.remove(' ');
    
    if (cleanNumber.isEmpty()) {
        return "Unknown";
    }
    
    // Visa: Starts with 4, length 16
    if (cleanNumber.startsWith('4')) {
        return "Visa";
    }
    
    // Mastercard: Starts with 51-55, length 16
    if (cleanNumber.length() > 1 && cleanNumber.startsWith('5') && 
        cleanNumber[1] >= '1' && cleanNumber[1] <= '5') {
        return "Mastercard";
    }
    
    // American Express: Starts with 34 or 37, length 15
    if (cleanNumber.length() > 1 && cleanNumber.startsWith('3') && 
        (cleanNumber[1] == '4' || cleanNumber[1] == '7')) {
        return "American Express";
    }
    
    // Discover: Starts with 6011, length 16
    if (cleanNumber.length() > 3 && cleanNumber.startsWith("6011")) {
        return "Discover";
    }
    
    return "Unknown";
}

void AddCardDialog::updateCardTypeLabel() {
    QString cardType = detectCardType(m_cardNumberEdit->text());
    m_cardTypeLabel->setText("Card Type: " + cardType);
}
