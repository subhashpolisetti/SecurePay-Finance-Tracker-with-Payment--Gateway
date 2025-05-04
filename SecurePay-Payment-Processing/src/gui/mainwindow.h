#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QStatusBar>
#include <QTabWidget>
#include <QStackedWidget>
#include <memory>
#include "../core/appcontroller.h"
#include "../core/transaction.h"
#include "../core/refundmanager.h"
#include "../core/reportmanager.h"

// Enum for user roles
enum class UserRole {
    CUSTOMER,
    MERCHANT
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private:
    // Core components
    std::unique_ptr<AppController> m_appController;
    RefundManager* m_refundManager;
    ReportManager* m_reportManager;
    
    // Role selection
    QComboBox* m_roleComboBox;
    QStackedWidget* m_mainStack;
    
    // Customer view widgets
    QWidget* m_customerView;
    QComboBox* m_customerComboBox;
    QPushButton* m_addCustomerButton;
    QLabel* m_customerDetailsLabel;
    QLineEdit* m_amountEdit;
    QComboBox* m_paymentMethodComboBox;
    QLineEdit* m_cardNumberEdit;
    QLineEdit* m_cardholderNameEdit;
    QLineEdit* m_expiryDateEdit;
    QLineEdit* m_cvvEdit;
    QLineEdit* m_walletIdEdit;
    QLineEdit* m_walletEmailEdit;
    QGroupBox* m_cardFieldsGroup;
    QGroupBox* m_walletFieldsGroup;
    QPushButton* m_submitButton;
    QLabel* m_resultLabel;
    QTableWidget* m_customerTransactionTable;
    QPushButton* m_exportCustomerReportButton;
    
    // Merchant view widgets
    QWidget* m_merchantView;
    QComboBox* m_merchantComboBox;
    QPushButton* m_addMerchantButton;
    QLabel* m_merchantDetailsLabel;
    QTabWidget* m_merchantTabs;
    QTableWidget* m_merchantTransactionTable;
    QTableWidget* m_refundTable;
    QTableWidget* m_fraudAlertTable;
    QPushButton* m_processRefundButton;
    QPushButton* m_exportMerchantReportButton;
    
    // UI setup methods
    void setupUI();
    void setupCustomerView();
    void setupMerchantView();
    
    // Update methods
    void updateCustomerDetails();
    void updateMerchantDetails();
    void updatePaymentMethodFields();
    void updateCustomerTransactionHistory();
    void updateMerchantTransactionHistory();
    void updateRefundHistory();
    void updateFraudAlerts();
    
private slots:
    // Role selection
    void onRoleChanged(int index);
    
    // Customer view slots
    void onCustomerSelected(int index);
    void onAddCustomerClicked();
    void onPaymentMethodSelected(int index);
    void onSubmitClicked();
    void onExportCustomerReportClicked();
    
    // Merchant view slots
    void onMerchantSelected(int index);
    void onAddMerchantClicked();
    void onProcessRefundClicked();
    void onExportMerchantReportClicked();
    
    // Common slots
    void onTransactionUpdated(const Transaction& transaction);
};

#endif
