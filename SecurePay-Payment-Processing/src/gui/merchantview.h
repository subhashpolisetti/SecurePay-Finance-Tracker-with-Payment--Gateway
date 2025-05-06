#ifndef MERCHANTVIEW_H
#define MERCHANTVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QTabWidget>
#include <memory>
#include "../core/appcontroller.h"
#include "../core/transaction.h"
#include "../core/refundmanager.h"
#include "../core/reportmanager.h"

/**
 * @class MerchantView
 * @brief Widget for the merchant view of the application
 * 
 * This class encapsulates all the UI elements and functionality
 * related to the merchant view of the application.
 */
class MerchantView : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param appController Reference to the application controller
     * @param parent Parent widget
     */
    MerchantView(AppController& appController, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MerchantView();
    
    /**
     * @brief Update the merchant details display
     */
    void updateMerchantDetails();
    
    /**
     * @brief Update the transaction history table
     */
    void updateTransactionHistory();
    
    /**
     * @brief Update the refund history table
     */
    void updateRefundHistory();
    
    /**
     * @brief Update the fraud alerts table
     */
    void updateFraudAlerts();
    
    /**
     * @brief Update the analytics display
     */
    void updateAnalytics();
    
public slots:
    /**
     * @brief Handle transaction update
     * @param transaction The updated transaction
     */
    void onTransactionUpdated(const Transaction& transaction);
    
private slots:
    /**
     * @brief Handle merchant selection change
     * @param index The selected index
     */
    void onMerchantSelected(int index);
    
    /**
     * @brief Handle add merchant button click
     */
    void onAddMerchantClicked();
    
    /**
     * @brief Handle process refund button click
     */
    void onProcessRefundClicked();
    
    /**
     * @brief Handle export merchant report button click
     */
    void onExportMerchantReportClicked();
    
private:
    /**
     * @brief Initialize the UI components
     */
    void initUI();
    
    // Core components
    AppController& m_appController;
    RefundManager* m_refundManager;
    ReportManager* m_reportManager;
    
    // Merchant information
    QComboBox* m_merchantComboBox;
    QPushButton* m_addMerchantButton;
    QLabel* m_merchantDetailsLabel;
    
    // Merchant tabs
    QTabWidget* m_merchantTabs;
    QTableWidget* m_transactionTable;
    QTableWidget* m_refundTable;
    QTableWidget* m_fraudAlertTable;
    
    // Analytics and reporting
    QGroupBox* m_analyticsGroup;
    QLabel* m_totalTransactionsLabel;
    QLabel* m_totalVolumeLabel;
    QLabel* m_avgTransactionLabel;
    QLabel* m_fraudRateLabel;
    QLabel* m_refundRateLabel;
    QPushButton* m_processRefundButton;
    QPushButton* m_exportReportButton;
};

#endif // MERCHANTVIEW_H
