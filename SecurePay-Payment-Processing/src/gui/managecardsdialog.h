#ifndef MANAGECARDSDIALOG_H
#define MANAGECARDSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <vector>
#include <memory>
#include "../core/cardtoken.h"
#include "addcarddialog.h"

/**
 * @class ManageCardsDialog
 * @brief Dialog for managing saved payment cards
 * 
 * This class provides a dialog for viewing, adding, and deleting saved payment cards.
 */
class ManageCardsDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param customerId The customer ID
     * @param parent The parent widget
     */
    explicit ManageCardsDialog(const std::string& customerId, QWidget* parent = nullptr);
    
    /**
     * @brief Get the selected card token
     * @return The selected card token, or nullptr if no card is selected
     */
    const CardToken* getSelectedCard() const;
    
private slots:
    /**
     * @brief Handle the Add Card button click
     */
    void onAddCardClicked();
    
    /**
     * @brief Handle the Delete Card button click
     */
    void onDeleteCardClicked();
    
    /**
     * @brief Handle the OK button click
     */
    void onOkClicked();
    
    /**
     * @brief Handle the Cancel button click
     */
    void onCancelClicked();
    
    /**
     * @brief Handle card selection changes
     * @param currentRow The current row
     */
    void onCardSelectionChanged(int currentRow);
    
private:
    /**
     * @brief Initialize the UI
     */
    void initUI();
    
    /**
     * @brief Load saved cards
     */
    void loadCards();
    
    QListWidget* m_cardListWidget;
    QPushButton* m_addCardButton;
    QPushButton* m_deleteCardButton;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    std::string m_customerId;
    std::vector<std::string> m_cardTokens; // Store token strings instead of pointers
    const CardToken* m_selectedCard;
};

#endif // MANAGECARDSDIALOG_H
