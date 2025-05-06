#include "managecardsdialog.h"

ManageCardsDialog::ManageCardsDialog(const std::string& customerId, QWidget* parent)
    : QDialog(parent), m_customerId(customerId), m_selectedCard(nullptr) {
    
    setWindowTitle("Manage Saved Cards");
    setMinimumWidth(500);
    setMinimumHeight(400);
    
    initUI();
    loadCards();
    
    connect(m_addCardButton, &QPushButton::clicked, this, &ManageCardsDialog::onAddCardClicked);
    connect(m_deleteCardButton, &QPushButton::clicked, this, &ManageCardsDialog::onDeleteCardClicked);
    connect(m_okButton, &QPushButton::clicked, this, &ManageCardsDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &ManageCardsDialog::onCancelClicked);
    connect(m_cardListWidget, &QListWidget::currentRowChanged, this, &ManageCardsDialog::onCardSelectionChanged);
}

const CardToken* ManageCardsDialog::getSelectedCard() const {
    return m_selectedCard;
}

void ManageCardsDialog::initUI() {
    // Create card list widget
    m_cardListWidget = new QListWidget();
    m_cardListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Create buttons
    m_addCardButton = new QPushButton("Add Card");
    m_deleteCardButton = new QPushButton("Delete Card");
    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");
    
    // Disable delete button initially
    m_deleteCardButton->setEnabled(false);
    
    // Create button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_addCardButton);
    buttonLayout->addWidget(m_deleteCardButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);
    
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(new QLabel("Saved Cards:"));
    mainLayout->addWidget(m_cardListWidget);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void ManageCardsDialog::loadCards() {
    // Clear the list widget and cards vector
    m_cardListWidget->clear();
    m_cardTokens.clear();
    
    // Get saved cards for the customer
    CardManager& cardManager = CardManager::getInstance();
    auto cards = cardManager.getCardTokensForCustomer(m_customerId);
    
    // Add cards to the list widget
    for (const auto& card : cards) {
        m_cardTokens.push_back(card->getToken());
        
        QString displayText = QString::fromStdString(card->getDisplayName());
        displayText += " (Expires: " + QString::fromStdString(card->getExpiryMonth()) + 
                      "/" + QString::fromStdString(card->getExpiryYear()) + ")";
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, QString::fromStdString(card->getToken()));
        m_cardListWidget->addItem(item);
    }
    
    // Add a message if no cards are found
    if (m_cardTokens.empty()) {
        m_cardListWidget->addItem("No saved cards found. Click 'Add Card' to add a new card.");
    }
}

void ManageCardsDialog::onAddCardClicked() {
    // Open the add card dialog
    AddCardDialog dialog(m_customerId, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Get the card token
        auto cardToken = dialog.getCardToken();
        if (cardToken) {
            // Add the card to the card manager
            CardManager& cardManager = CardManager::getInstance();
            cardManager.addCardToken(std::move(cardToken));
            
            // Reload the cards
            loadCards();
        }
    }
}

void ManageCardsDialog::onDeleteCardClicked() {
    // Get the selected card
    int currentRow = m_cardListWidget->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(m_cardTokens.size())) {
        // Get the token string
        QString tokenString = m_cardListWidget->item(currentRow)->data(Qt::UserRole).toString();
        std::string token = tokenString.toUtf8().constData();
        
        // Find the card token with this token string
        CardManager& cardManager = CardManager::getInstance();
        const CardToken* card = nullptr;
        auto cards = cardManager.getCardTokensForCustomer(m_customerId);
        for (const auto* c : cards) {
            if (c->getToken() == token) {
                card = c;
                break;
            }
        }
        
        if (!card) {
            QMessageBox::warning(this, "Error", "Card not found.");
            return;
        }
        
        // Confirm deletion
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Confirm Deletion", 
            "Are you sure you want to delete the card " + QString::fromStdString(card->getDisplayName()) + "?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            // Delete the card
            cardManager.deleteCardToken(token);
            
            // Reload the cards
            loadCards();
            
            // Clear the selected card
            m_selectedCard = nullptr;
            
            // Disable the delete button
            m_deleteCardButton->setEnabled(false);
        }
    }
}

void ManageCardsDialog::onOkClicked() {
    // Get the selected card
    int currentRow = m_cardListWidget->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(m_cardTokens.size())) {
        // Get the token string
        QString tokenString = m_cardListWidget->item(currentRow)->data(Qt::UserRole).toString();
        std::string token = tokenString.toUtf8().constData();
        
        // Find the card token with this token string
        CardManager& cardManager = CardManager::getInstance();
        auto cards = cardManager.getCardTokensForCustomer(m_customerId);
        for (const auto* c : cards) {
            if (c->getToken() == token) {
                m_selectedCard = c;
                break;
            }
        }
        
        if (m_selectedCard) {
            accept();
        } else {
            QMessageBox::warning(this, "Error", "Card not found.");
        }
    } else {
        QMessageBox::warning(this, "No Card Selected", "Please select a card or add a new one.");
    }
}

void ManageCardsDialog::onCancelClicked() {
    reject();
}

void ManageCardsDialog::onCardSelectionChanged(int currentRow) {
    // Enable or disable the delete button based on selection
    m_deleteCardButton->setEnabled(currentRow >= 0 && currentRow < static_cast<int>(m_cardTokens.size()));
}
