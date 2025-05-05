#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include "../core/appcontroller.h"

/**
 * @class LoginDialog
 * @brief Dialog for user login and registration
 * 
 * This class provides a UI for user login and registration.
 * It follows the Single Responsibility Principle by focusing only on
 * authentication and registration.
 */
class LoginDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param appController Pointer to the application controller
     * @param parent Parent widget
     */
    LoginDialog(AppController* appController, QWidget* parent = nullptr);
    
    /**
     * @brief Get the authenticated customer
     * @return The authenticated customer, or nullptr if not authenticated
     */
    const Customer* getAuthenticatedCustomer() const;
    
    /**
     * @brief Destructor
     */
    ~LoginDialog();
    
private:
    // Core components
    AppController* m_appController;
    const Customer* m_authenticatedCustomer;
    
    // UI components
    QStackedWidget* m_stackedWidget;
    
    // Login page
    QWidget* m_loginPage;
    QLineEdit* m_usernameOrEmailEdit;
    QLineEdit* m_pinEdit;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
    
    // Register page
    QWidget* m_registerPage;
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_usernameEdit;
    QLineEdit* m_pinRegisterEdit;
    QLineEdit* m_confirmPinEdit;
    QLineEdit* m_addressEdit;
    QPushButton* m_createAccountButton;
    QPushButton* m_backToLoginButton;
    
    /**
     * @brief Initialize the UI components
     */
    void initUI();
    
    /**
     * @brief Validate the login form
     * @return True if all inputs are valid, false otherwise
     */
    bool validateLoginForm();
    
    /**
     * @brief Validate the registration form
     * @return True if all inputs are valid, false otherwise
     */
    bool validateRegistrationForm();
    
private slots:
    /**
     * @brief Handle login button click
     */
    void onLoginClicked();
    
    /**
     * @brief Handle register button click
     */
    void onRegisterClicked();
    
    /**
     * @brief Handle create account button click
     */
    void onCreateAccountClicked();
    
    /**
     * @brief Handle back to login button click
     */
    void onBackToLoginClicked();
};

#endif // LOGINDIALOG_H
