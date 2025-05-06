#include "mainwindow.h"
#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <functional>
#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_appController(AppController::getInstance()) {
    // Setup UI
    setupUI();
    
    // Register for transaction updates
    m_appController.setTransactionUpdateCallback(
        std::bind(&MainWindow::onTransactionUpdated, this, std::placeholders::_1));
    
    setWindowTitle("SecurePay Payment Processing");
    setMinimumSize(1000, 700);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Set object names for debugging
    setObjectName("MainWindow");
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Create role selection toolbar
    QToolBar* roleToolbar = new QToolBar("Role Selection", this);
    roleToolbar->setObjectName("roleToolbar");
    roleToolbar->setMovable(false);
    roleToolbar->setFloatable(false);
    
    QLabel* roleLabel = new QLabel("Select Role: ");
    roleLabel->setObjectName("roleLabel");
    roleToolbar->addWidget(roleLabel);
    
    m_roleComboBox = new QComboBox(roleToolbar);
    m_roleComboBox->setObjectName("roleComboBox");
    m_roleComboBox->addItem("Customer", static_cast<int>(UserRole::CUSTOMER));
    m_roleComboBox->addItem("Merchant", static_cast<int>(UserRole::MERCHANT));
    roleToolbar->addWidget(m_roleComboBox);
    
    addToolBar(roleToolbar);
    
    // Create stacked widget for different role views
    m_mainStack = new QStackedWidget(centralWidget);
    m_mainStack->setObjectName("mainStack");
    m_mainStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Create customer view with scroll area
    QScrollArea* customerScrollArea = new QScrollArea(m_mainStack);
    customerScrollArea->setObjectName("customerScrollArea");
    customerScrollArea->setWidgetResizable(true);
    customerScrollArea->setFrameShape(QFrame::NoFrame);
    customerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    customerScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_customerView = new CustomerView(m_appController, customerScrollArea);
    m_customerView->setObjectName("customerView");
    customerScrollArea->setWidget(m_customerView);
    m_mainStack->addWidget(customerScrollArea);
    
    // Create merchant view with scroll area
    QScrollArea* merchantScrollArea = new QScrollArea(m_mainStack);
    merchantScrollArea->setObjectName("merchantScrollArea");
    merchantScrollArea->setWidgetResizable(true);
    merchantScrollArea->setFrameShape(QFrame::NoFrame);
    merchantScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    merchantScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_merchantView = new MerchantView(m_appController, merchantScrollArea);
    m_merchantView->setObjectName("merchantView");
    merchantScrollArea->setWidget(m_merchantView);
    m_mainStack->addWidget(merchantScrollArea);
    
    // Add stacked widget to main layout
    mainLayout->addWidget(m_mainStack);
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Set status bar
    statusBar()->showMessage("Ready");
    
    // Connect signals
    connect(m_roleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRoleChanged);
    
    // Connect transaction updated signal from views to this class
    connect(m_customerView, &CustomerView::transactionUpdated,
            this, &MainWindow::onTransactionUpdated);
    
    // Set initial role to Customer
    m_roleComboBox->setCurrentIndex(0);
    onRoleChanged(0);
    
    // Set minimum size for the window
    setMinimumSize(1000, 700);
}

void MainWindow::onRoleChanged(int index) {
    UserRole role = static_cast<UserRole>(m_roleComboBox->itemData(index).toInt());
    
    switch (role) {
        case UserRole::CUSTOMER:
            m_mainStack->setCurrentIndex(0); // Customer view
            statusBar()->showMessage("Customer Mode");
            break;
            
        case UserRole::MERCHANT:
            m_mainStack->setCurrentIndex(1); // Merchant view
            statusBar()->showMessage("Merchant Mode");
            break;
    }
    
    // Force layout update and repaint
    if (QLayout* layout = centralWidget()->layout()) {
        layout->invalidate();
        layout->activate();
    }
    
    // Update the UI
    update();
    
    // Update the entire window
    update();
    repaint();
}

void MainWindow::onTransactionUpdated(const Transaction& transaction) {
    // Forward the transaction update to the merchant view
    m_merchantView->onTransactionUpdated(transaction);
    
    // Update status bar
    statusBar()->showMessage("Transaction updated: " + QString::fromUtf8(transaction.getTransactionId().c_str()));
}
