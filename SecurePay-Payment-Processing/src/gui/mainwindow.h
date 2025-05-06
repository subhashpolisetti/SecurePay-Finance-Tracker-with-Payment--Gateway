#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QStatusBar>
#include <QStackedWidget>
#include <QScrollArea>
#include <memory>
#include "../core/appcontroller.h"
#include "../core/transaction.h"
#include "customerview.h"
#include "merchantview.h"

// Enum for user roles
enum class UserRole {
    CUSTOMER,
    MERCHANT
};

/**
 * @class MainWindow
 * @brief Main window of the application
 * 
 * This class is the main window of the application. It contains
 * a stacked widget with customer and merchant views.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    MainWindow(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MainWindow();
    
private:
    /**
     * @brief Initialize the UI components
     */
    void setupUI();
    
    // Core components
    AppController& m_appController;
    
    // Role selection
    QComboBox* m_roleComboBox;
    QStackedWidget* m_mainStack;
    
    // Views
    CustomerView* m_customerView;
    MerchantView* m_merchantView;
    
private slots:
    /**
     * @brief Handle role selection change
     * @param index The selected index
     */
    void onRoleChanged(int index);
    
    /**
     * @brief Handle transaction update
     * @param transaction The updated transaction
     */
    void onTransactionUpdated(const Transaction& transaction);
};

#endif // MAINWINDOW_H
