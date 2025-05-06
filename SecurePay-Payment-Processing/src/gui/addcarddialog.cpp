#include "addcarddialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDate>

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
    
    // Only allow digits and spaces for exactly 16 digits
    QRegularExpression cardRegex("[0-9 ]{0,19}"); // Allow up to 19 chars with spaces for 16 digits
    QRegularExpressionValidator* cardValidator = new QRegularExpressionValidator(cardRegex, this);
    m_cardNumberEdit->setValidator(cardValidator);
    
    formLayout->addRow("Card Number:", m_cardNumberEdit);
    
    // Card type label
    m_cardTypeLabel = new QLabel("Card Type: Unknown");
    formLayout->addRow("", m_cardTypeLabel);
    
    // Card category combo box
    m_cardCategoryComboBox = new QComboBox();
    m_cardCategoryComboBox->addItem("Credit Card", static_cast<int>(CardCategory::CREDIT));
    m_cardCategoryComboBox->addItem("Debit Card", static_cast<int>(CardCategory::DEBIT));
    formLayout->addRow("Card Category:", m_cardCategoryComboBox);
    
    // Cardholder name field
    m_cardholderNameEdit = new QLineEdit();
    m_cardholderNameEdit->setPlaceholderText("John Doe");
    formLayout->addRow("Cardholder Name:", m_cardholderNameEdit);
    
    // Expiry date field
    m_expiryDateEdit = new QLineEdit();
    m_expiryDateEdit->setPlaceholderText("MM/YY");
    m_expiryDateEdit->setInputMask("99/99;_"); // Add ;_ to prevent space at beginning
    
    formLayout->addRow("Expiry Date (MM/YY):", m_expiryDateEdit);
    
    // CVV field
    m_cvvEdit = new QLineEdit();
    m_cvvEdit->setPlaceholderText("123");
    m_cvvEdit->setMaxLength(3);  // Limit to exactly 3 digits
    m_cvvEdit->setEchoMode(QLineEdit::Password);
    
    // Only allow exactly 3 digits
    QRegularExpression cvvRegex("[0-9]{3}");
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
    
    // Get the selected card category
    CardCategory category = static_cast<CardCategory>(
        m_cardCategoryComboBox->currentData().toInt());
    
    // Create a card token
    m_cardToken = std::make_unique<CardToken>(
        cardNumber.toUtf8().constData(),
        cardholderName.toUtf8().constData(),
        expiryDate.toUtf8().constData(),
        m_customerId,
        category);
    
    accept();
}

void AddCardDialog::onCancelClicked() {
    reject();
}

void AddCardDialog::onCardNumberChanged(const QString& text) {
    // Disconnect the signal temporarily to prevent recursion
    disconnect(m_cardNumberEdit, &QLineEdit::textChanged, this, &AddCardDialog::onCardNumberChanged);
    
    // Get the current cursor position
    int cursorPos = m_cardNumberEdit->cursorPosition();
    
    // Remove all spaces from the text
    QString cleanText = text;
    cleanText.remove(' ');
    
    // Calculate how many digits are before the cursor
    int digitsBeforeCursor = 0;
    for (int i = 0; i < cursorPos && i < text.length(); ++i) {
        if (text[i] != ' ') {
            digitsBeforeCursor++;
        }
    }
    
    // Format the clean text with spaces
    QString formattedText;
    for (int i = 0; i < cleanText.length(); ++i) {
        // Add a space after every 4th digit (but not at the beginning)
        if (i > 0 && i % 4 == 0) {
            formattedText += ' ';
        }
        formattedText += cleanText[i];
    }
    
    // Calculate the new cursor position
    int newCursorPos = 0;
    int digitCount = 0;
    for (int i = 0; i < formattedText.length(); ++i) {
        if (digitCount == digitsBeforeCursor) {
            newCursorPos = i;
            break;
        }
        
        if (formattedText[i] != ' ') {
            digitCount++;
        }
    }
    
    // If we reached the end, set cursor at the end
    if (digitCount == digitsBeforeCursor) {
        newCursorPos = formattedText.length();
    }
    
    // Set the formatted text and cursor position
    m_cardNumberEdit->setText(formattedText);
    m_cardNumberEdit->setCursorPosition(newCursorPos);
    
    // Update the card type label
    updateCardTypeLabel();
    
    // Reconnect the signal
    connect(m_cardNumberEdit, &QLineEdit::textChanged, this, &AddCardDialog::onCardNumberChanged);
}

bool AddCardDialog::validateForm() {
    // Check card number
    QString cardNumber = m_cardNumberEdit->text().remove(' ');
    if (cardNumber.length() != 16) {
        QMessageBox::warning(this, "Invalid Card", "Please enter a valid 16-digit card number.");
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
    
    // Validate expiry date is in the future
    QStringList expiryParts = m_expiryDateEdit->text().split('/');
    if (expiryParts.size() == 2) {
        int month = expiryParts[0].toInt();
        int year = 2000 + expiryParts[1].toInt(); // Convert YY to YYYY
        
        QDate expiryDate(year, month, 1);
        QDate currentDate = QDate::currentDate();
        
        if (expiryDate <= currentDate) {
            QMessageBox::warning(this, "Invalid Expiry Date", "The card has expired. Please enter a valid future date.");
            return false;
        }
    }
    
    // Check CVV
    QString cvv = m_cvvEdit->text();
    if (cvv.length() != 3) {
        QMessageBox::warning(this, "Invalid CVV", "Please enter a valid 3-digit CVV code.");
        return false;
    }
    
    // Check for duplicate cards
    CardManager& cardManager = CardManager::getInstance();
    auto existingCards = cardManager.getCardTokensForCustomer(m_customerId);
    
    for (const auto* card : existingCards) {
        // Check if the last 4 digits match
        if (cardNumber.right(4) == QString::fromStdString(card->getLastFourDigits())) {
            // Check if the cardholder name matches
            if (m_cardholderNameEdit->text().toUtf8().constData() == card->getCardholderName()) {
                QMessageBox::warning(this, "Duplicate Card", 
                    "This card appears to be already saved. Please use the existing card or enter a different one.");
                return false;
            }
        }
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
