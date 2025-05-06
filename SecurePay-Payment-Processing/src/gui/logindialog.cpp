#include "logindialog.h"
#include <QRegularExpressionValidator>

LoginDialog::LoginDialog(AppController* appController, QWidget* parent)
    : QDialog(parent), m_appController(appController), m_authenticatedCustomer(nullptr) {
    
    setWindowTitle("SecurePay - Login/Register");
    setMinimumSize(400, 300);
    
    // Initialize UI
    initUI();
    
    // Connect signals and slots
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(m_createAccountButton, &QPushButton::clicked, this, &LoginDialog::onCreateAccountClicked);
    connect(m_backToLoginButton, &QPushButton::clicked, this, &LoginDialog::onBackToLoginClicked);
}

LoginDialog::~LoginDialog() {
}

const Customer* LoginDialog::getAuthenticatedCustomer() const {
    return m_authenticatedCustomer;
}

void LoginDialog::initUI() {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create stacked widget for login and register pages
    m_stackedWidget = new QStackedWidget(this);
    
    // Create login page
    m_loginPage = new QWidget(m_stackedWidget);
    QVBoxLayout* loginLayout = new QVBoxLayout(m_loginPage);
    
    QLabel* loginTitle = new QLabel("Login to SecurePay", m_loginPage);
    loginTitle->setStyleSheet("font-size: 16pt; font-weight: bold;");
    loginTitle->setAlignment(Qt::AlignCenter);
    
    QFormLayout* loginFormLayout = new QFormLayout();
    
    m_usernameOrEmailEdit = new QLineEdit(m_loginPage);
    m_usernameOrEmailEdit->setPlaceholderText("Enter username or email");
    
    m_pinEdit = new QLineEdit(m_loginPage);
    m_pinEdit->setPlaceholderText("Enter PIN");
    m_pinEdit->setEchoMode(QLineEdit::Password);
    
    loginFormLayout->addRow("Username/Email:", m_usernameOrEmailEdit);
    loginFormLayout->addRow("PIN:", m_pinEdit);
    
    QHBoxLayout* loginButtonLayout = new QHBoxLayout();
    
    m_loginButton = new QPushButton("Login", m_loginPage);
    m_loginButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    m_loginButton->setMinimumHeight(40);
    
    m_registerButton = new QPushButton("Register", m_loginPage);
    m_registerButton->setMinimumHeight(40);
    
    loginButtonLayout->addWidget(m_loginButton);
    loginButtonLayout->addWidget(m_registerButton);
    
    loginLayout->addWidget(loginTitle);
    loginLayout->addLayout(loginFormLayout);
    loginLayout->addLayout(loginButtonLayout);
    
    // Create register page
    m_registerPage = new QWidget(m_stackedWidget);
    QVBoxLayout* registerLayout = new QVBoxLayout(m_registerPage);
    
    QLabel* registerTitle = new QLabel("Register for SecurePay", m_registerPage);
    registerTitle->setStyleSheet("font-size: 16pt; font-weight: bold;");
    registerTitle->setAlignment(Qt::AlignCenter);
    
    QFormLayout* registerFormLayout = new QFormLayout();
    
    m_nameEdit = new QLineEdit(m_registerPage);
    m_nameEdit->setPlaceholderText("Enter your full name");
    
    m_emailEdit = new QLineEdit(m_registerPage);
    m_emailEdit->setPlaceholderText("Enter your email");
    
    m_usernameEdit = new QLineEdit(m_registerPage);
    m_usernameEdit->setPlaceholderText("Choose a username");
    
    m_pinRegisterEdit = new QLineEdit(m_registerPage);
    m_pinRegisterEdit->setPlaceholderText("Choose a PIN");
    m_pinRegisterEdit->setEchoMode(QLineEdit::Password);
    
    m_confirmPinEdit = new QLineEdit(m_registerPage);
    m_confirmPinEdit->setPlaceholderText("Confirm your PIN");
    m_confirmPinEdit->setEchoMode(QLineEdit::Password);
    
    m_addressEdit = new QLineEdit(m_registerPage);
    m_addressEdit->setPlaceholderText("Enter your address");
    
    registerFormLayout->addRow("Name:", m_nameEdit);
    registerFormLayout->addRow("Email:", m_emailEdit);
    registerFormLayout->addRow("Username:", m_usernameEdit);
    registerFormLayout->addRow("PIN:", m_pinRegisterEdit);
    registerFormLayout->addRow("Confirm PIN:", m_confirmPinEdit);
    registerFormLayout->addRow("Address:", m_addressEdit);
    
    QHBoxLayout* registerButtonLayout = new QHBoxLayout();
    
    m_createAccountButton = new QPushButton("Create Account", m_registerPage);
    m_createAccountButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    m_createAccountButton->setMinimumHeight(40);
    
    m_backToLoginButton = new QPushButton("Back to Login", m_registerPage);
    m_backToLoginButton->setMinimumHeight(40);
    
    registerButtonLayout->addWidget(m_createAccountButton);
    registerButtonLayout->addWidget(m_backToLoginButton);
    
    registerLayout->addWidget(registerTitle);
    registerLayout->addLayout(registerFormLayout);
    registerLayout->addLayout(registerButtonLayout);
    
    // Add pages to stacked widget
    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_registerPage);
    
    // Set initial page to login
    m_stackedWidget->setCurrentIndex(0);
    
    // Add stacked widget to main layout
    mainLayout->addWidget(m_stackedWidget);
}

bool LoginDialog::validateLoginForm() {
    // Check if username/email is empty
    if (m_usernameOrEmailEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a username or email.");
        m_usernameOrEmailEdit->setFocus();
        return false;
    }
    
    // Check if PIN is empty
    if (m_pinEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a PIN.");
        m_pinEdit->setFocus();
        return false;
    }
    
    return true;
}

bool LoginDialog::validateRegistrationForm() {
    // Check if name is empty
    if (m_nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter your name.");
        m_nameEdit->setFocus();
        return false;
    }
    
    // Check if email is empty
    if (m_emailEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter your email.");
        m_emailEdit->setFocus();
        return false;
    }
    
    // Check if username is empty
    if (m_usernameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please choose a username.");
        m_usernameEdit->setFocus();
        return false;
    }
    
    // Check if PIN is empty
    if (m_pinRegisterEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please choose a PIN.");
        m_pinRegisterEdit->setFocus();
        return false;
    }
    
    // Check if confirm PIN is empty
    if (m_confirmPinEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please confirm your PIN.");
        m_confirmPinEdit->setFocus();
        return false;
    }
    
    // Check if PINs match
    if (m_pinRegisterEdit->text() != m_confirmPinEdit->text()) {
        QMessageBox::warning(this, "Validation Error", "PINs do not match.");
        m_confirmPinEdit->setFocus();
        return false;
    }
    
    // Check if address is empty
    if (m_addressEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter your address.");
        m_addressEdit->setFocus();
        return false;
    }
    
    return true;
}

void LoginDialog::onLoginClicked() {
    if (!validateLoginForm()) {
        return;
    }
    
    // Get username/email and PIN
    QString usernameOrEmail = m_usernameOrEmailEdit->text();
    QString pin = m_pinEdit->text();
    
    // Authenticate customer
    bool authenticated = false;
    for (const auto& customer : m_appController->getCustomers()) {
        if (customer.authenticate(usernameOrEmail.toUtf8().constData(), pin.toUtf8().constData())) {
            m_authenticatedCustomer = &customer;
            authenticated = true;
            break;
        }
    }
    
    if (authenticated) {
        // Set the authenticated customer in the AppController
        m_appController->setAuthenticatedCustomer(m_authenticatedCustomer);
        QMessageBox::information(this, "Login Successful", "You have successfully logged in.");
        accept(); // Close dialog with accept result
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username/email or PIN.");
        m_pinEdit->clear();
        m_pinEdit->setFocus();
    }
}

void LoginDialog::onRegisterClicked() {
    // Switch to register page
    m_stackedWidget->setCurrentIndex(1);
}

void LoginDialog::onCreateAccountClicked() {
    if (!validateRegistrationForm()) {
        return;
    }
    
    // Get registration details
    QString name = m_nameEdit->text();
    QString email = m_emailEdit->text();
    QString username = m_usernameEdit->text();
    QString pin = m_pinRegisterEdit->text();
    QString address = m_addressEdit->text();
    
    // Check if username or email already exists
    for (const auto& customer : m_appController->getCustomers()) {
        if (customer.getUsername() == username.toUtf8().constData()) {
            QMessageBox::warning(this, "Registration Error", "Username already exists. Please choose a different username.");
            m_usernameEdit->setFocus();
            return;
        }
        
        if (customer.getEmail() == email.toUtf8().constData()) {
            QMessageBox::warning(this, "Registration Error", "Email already exists. Please use a different email or login with your existing account.");
            m_emailEdit->setFocus();
            return;
        }
    }
    
    // Create new customer
    Customer newCustomer(name.toUtf8().constData(), email.toUtf8().constData(), 
                         username.toUtf8().constData(), pin.toUtf8().constData(), 
                         address.toUtf8().constData());
    
    // Add customer to app controller
    m_appController->addCustomer(newCustomer);
    
    // Set authenticated customer to the newly created customer
    // Note: We need to get a reference to the customer in the app controller's list
    for (const auto& customer : m_appController->getCustomers()) {
        if (customer.getUsername() == username.toUtf8().constData()) {
            m_authenticatedCustomer = &customer;
            break;
        }
    }
    
    // Set the authenticated customer in the AppController
    m_appController->setAuthenticatedCustomer(m_authenticatedCustomer);
    
    QMessageBox::information(this, "Registration Successful", "Your account has been created successfully.");
    accept(); // Close dialog with accept result
}

void LoginDialog::onBackToLoginClicked() {
    // Switch to login page
    m_stackedWidget->setCurrentIndex(0);
}
