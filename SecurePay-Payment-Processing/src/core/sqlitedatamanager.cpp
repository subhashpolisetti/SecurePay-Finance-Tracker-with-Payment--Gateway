#include "sqlitedatamanager.h"
#include <iostream>
#include <sstream>

SQLiteDataManager::SQLiteDataManager(const std::string& dbPath)
    : m_dbPath(dbPath), m_db(nullptr) {
}

SQLiteDataManager::~SQLiteDataManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool SQLiteDataManager::initialize() {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }
    
    std::cout << "SQLite database opened successfully: " << m_dbPath << std::endl;
    
    return createTables();
}

bool SQLiteDataManager::createTables() {
    // Create customers table
    std::string sql = "CREATE TABLE IF NOT EXISTS customers ("
                      "name TEXT PRIMARY KEY, "
                      "email TEXT, "
                      "billing_address TEXT"
                      ");";
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    // Create merchants table
    sql = "CREATE TABLE IF NOT EXISTS merchants ("
          "name TEXT PRIMARY KEY, "
          "email TEXT, "
          "business_address TEXT"
          ");";
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    // Create transactions table
    sql = "CREATE TABLE IF NOT EXISTS transactions ("
          "id TEXT PRIMARY KEY, "
          "customer_name TEXT, "
          "merchant_name TEXT, "
          "amount REAL, "
          "refunded_amount REAL, "
          "status INTEGER, "
          "timestamp TEXT, "
          "payment_method_type TEXT, "
          "payment_detail1 TEXT, "
          "payment_detail2 TEXT, "
          "payment_detail3 TEXT, "
          "payment_detail4 TEXT, "
          "FOREIGN KEY (customer_name) REFERENCES customers (name), "
          "FOREIGN KEY (merchant_name) REFERENCES merchants (name)"
          ");";
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    // Create refunds table
    sql = "CREATE TABLE IF NOT EXISTS refunds ("
          "id TEXT PRIMARY KEY, "
          "transaction_id TEXT, "
          "amount REAL, "
          "reason TEXT, "
          "timestamp TEXT, "
          "FOREIGN KEY (transaction_id) REFERENCES transactions (id)"
          ");";
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    // Create fraud_alerts table
    sql = "CREATE TABLE IF NOT EXISTS fraud_alerts ("
          "id TEXT PRIMARY KEY, "
          "transaction_id TEXT, "
          "risk_level INTEGER, "
          "description TEXT, "
          "timestamp TEXT, "
          "reviewed INTEGER, "
          "FOREIGN KEY (transaction_id) REFERENCES transactions (id)"
          ");";
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    std::cout << "Database tables created successfully" << std::endl;
    return true;
}

bool SQLiteDataManager::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool SQLiteDataManager::executeQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), callback, data, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL query error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool SQLiteDataManager::saveAll() {
    // This would be implemented to save all data in memory to the database
    // For now, we'll just return true
    return true;
}

bool SQLiteDataManager::loadAll() {
    // This would be implemented to load all data from the database into memory
    // For now, we'll just return true
    return true;
}

bool SQLiteDataManager::saveCustomer(const Customer& customer) {
    std::stringstream ss;
    ss << "INSERT OR REPLACE INTO customers (name, email, billing_address) VALUES ("
       << "'" << customer.getName() << "', "
       << "'" << customer.getEmail() << "', "
       << "'" << customer.getBillingAddress() << "'"
       << ");";
    
    return executeSQL(ss.str());
}

// Callback function for loadCustomers
static int customerCallback(void* data, int argc, char** argv, char** azColName) {
    std::vector<Customer>* customers = static_cast<std::vector<Customer>*>(data);
    
    std::string name = argv[0] ? argv[0] : "";
    std::string email = argv[1] ? argv[1] : "";
    std::string billingAddress = argv[2] ? argv[2] : "";
    
    customers->emplace_back(name, email, billingAddress);
    
    return 0;
}

std::vector<Customer> SQLiteDataManager::loadCustomers() {
    std::vector<Customer> customers;
    
    std::string sql = "SELECT name, email, billing_address FROM customers;";
    
    if (!executeQuery(sql, customerCallback, &customers)) {
        std::cerr << "Failed to load customers" << std::endl;
    }
    
    return customers;
}

bool SQLiteDataManager::saveMerchant(const Merchant& merchant) {
    std::stringstream ss;
    ss << "INSERT OR REPLACE INTO merchants (name, email, business_address) VALUES ("
       << "'" << merchant.getName() << "', "
       << "'" << merchant.getEmail() << "', "
       << "'" << merchant.getBusinessAddress() << "'"
       << ");";
    
    return executeSQL(ss.str());
}

// Callback function for loadMerchants
static int merchantCallback(void* data, int argc, char** argv, char** azColName) {
    std::vector<Merchant>* merchants = static_cast<std::vector<Merchant>*>(data);
    
    std::string name = argv[0] ? argv[0] : "";
    std::string email = argv[1] ? argv[1] : "";
    std::string businessAddress = argv[2] ? argv[2] : "";
    
    merchants->emplace_back(name, email, businessAddress);
    
    return 0;
}

std::vector<Merchant> SQLiteDataManager::loadMerchants() {
    std::vector<Merchant> merchants;
    
    std::string sql = "SELECT name, email, business_address FROM merchants;";
    
    if (!executeQuery(sql, merchantCallback, &merchants)) {
        std::cerr << "Failed to load merchants" << std::endl;
    }
    
    return merchants;
}

bool SQLiteDataManager::saveTransaction(const Transaction& transaction) {
    std::stringstream ss;
    ss << "INSERT OR REPLACE INTO transactions ("
       << "id, customer_name, merchant_name, amount, refunded_amount, status, timestamp, "
       << "payment_method_type, payment_detail1, payment_detail2, payment_detail3, payment_detail4"
       << ") VALUES ("
       << "'" << transaction.getTransactionId() << "', "
       << "'" << transaction.getCustomer().getName() << "', "
       << "'" << transaction.getMerchant().getName() << "', "
       << transaction.getAmount() << ", "
       << transaction.getRefundedAmount() << ", "
       << static_cast<int>(transaction.getStatus()) << ", "
       << "'" << transaction.getTimestamp() << "', "
       << "'" << transaction.getPaymentMethod().getType() << "', ";
    
    // Payment method details would be extracted here
    // For simplicity, we'll just use empty strings for now
    ss << "'', '', '', ''"
       << ");";
    
    return executeSQL(ss.str());
}

// Structure to hold transaction data during loading
struct TransactionData {
    std::vector<std::unique_ptr<Transaction>>* transactions;
    const std::vector<Customer>* customers;
    const std::vector<Merchant>* merchants;
    SQLiteDataManager* manager;
};

// Callback function for loadTransactions
static int transactionCallback(void* data, int argc, char** argv, char** azColName) {
    TransactionData* transactionData = static_cast<TransactionData*>(data);
    
    std::string id = argv[0] ? argv[0] : "";
    std::string customerName = argv[1] ? argv[1] : "";
    std::string merchantName = argv[2] ? argv[2] : "";
    double amount = argv[3] ? std::stod(argv[3]) : 0.0;
    double refundedAmount = argv[4] ? std::stod(argv[4]) : 0.0;
    int status = argv[5] ? std::stoi(argv[5]) : 0;
    std::string timestamp = argv[6] ? argv[6] : "";
    std::string paymentMethodType = argv[7] ? argv[7] : "";
    std::string paymentDetail1 = argv[8] ? argv[8] : "";
    std::string paymentDetail2 = argv[9] ? argv[9] : "";
    std::string paymentDetail3 = argv[10] ? argv[10] : "";
    std::string paymentDetail4 = argv[11] ? argv[11] : "";
    
    // Find the customer and merchant
    const Customer* customer = transactionData->manager->findCustomerByName(customerName, *transactionData->customers);
    const Merchant* merchant = transactionData->manager->findMerchantByName(merchantName, *transactionData->merchants);
    
    if (!customer || !merchant) {
        std::cerr << "Failed to find customer or merchant for transaction " << id << std::endl;
        return 0;
    }
    
    // Create the payment method
    auto paymentMethod = transactionData->manager->createPaymentMethod(
        paymentMethodType, paymentDetail1, paymentDetail2, paymentDetail3, paymentDetail4);
    
    if (!paymentMethod) {
        std::cerr << "Failed to create payment method for transaction " << id << std::endl;
        return 0;
    }
    
    // Create the transaction
    auto transaction = TransactionFactory::createTransaction(*customer, *merchant, std::move(paymentMethod), amount);
    
    // Set the transaction state based on the status
    // This would need to be implemented based on the State pattern
    
    transactionData->transactions->push_back(std::move(transaction));
    
    return 0;
}

std::vector<std::unique_ptr<Transaction>> SQLiteDataManager::loadTransactions(
    const std::vector<Customer>& customers,
    const std::vector<Merchant>& merchants) {
    
    std::vector<std::unique_ptr<Transaction>> transactions;
    
    TransactionData data;
    data.transactions = &transactions;
    data.customers = &customers;
    data.merchants = &merchants;
    data.manager = this;
    
    std::string sql = "SELECT id, customer_name, merchant_name, amount, refunded_amount, status, timestamp, "
                      "payment_method_type, payment_detail1, payment_detail2, payment_detail3, payment_detail4 "
                      "FROM transactions;";
    
    if (!executeQuery(sql, transactionCallback, &data)) {
        std::cerr << "Failed to load transactions" << std::endl;
    }
    
    return transactions;
}

bool SQLiteDataManager::saveRefund(const Refund& refund) {
    std::stringstream ss;
    ss << "INSERT OR REPLACE INTO refunds (id, transaction_id, amount, reason, timestamp) VALUES ("
       << "'" << refund.getRefundId() << "', "
       << "'" << refund.getTransaction().getTransactionId() << "', "
       << refund.getAmount() << ", "
       << "'" << refund.getReason() << "', "
       << "'" << refund.getTimestamp() << "'"
       << ");";
    
    return executeSQL(ss.str());
}

// Structure to hold refund data during loading
struct RefundData {
    std::vector<std::unique_ptr<Refund>>* refunds;
    const std::vector<std::unique_ptr<Transaction>>* transactions;
    SQLiteDataManager* manager;
};

// Callback function for loadRefunds
static int refundCallback(void* data, int argc, char** argv, char** azColName) {
    RefundData* refundData = static_cast<RefundData*>(data);
    
    std::string id = argv[0] ? argv[0] : "";
    std::string transactionId = argv[1] ? argv[1] : "";
    double amount = argv[2] ? std::stod(argv[2]) : 0.0;
    std::string reason = argv[3] ? argv[3] : "";
    std::string timestamp = argv[4] ? argv[4] : "";
    
    // Find the transaction
    const Transaction* transaction = refundData->manager->findTransactionById(transactionId, *refundData->transactions);
    
    if (!transaction) {
        std::cerr << "Failed to find transaction for refund " << id << std::endl;
        return 0;
    }
    
    // Create the refund
    auto refund = RefundFactory::createRefund(*transaction, amount, reason);
    
    refundData->refunds->push_back(std::move(refund));
    
    return 0;
}

std::vector<std::unique_ptr<Refund>> SQLiteDataManager::loadRefunds(
    const std::vector<std::unique_ptr<Transaction>>& transactions) {
    
    std::vector<std::unique_ptr<Refund>> refunds;
    
    RefundData data;
    data.refunds = &refunds;
    data.transactions = &transactions;
    data.manager = this;
    
    std::string sql = "SELECT id, transaction_id, amount, reason, timestamp FROM refunds;";
    
    if (!executeQuery(sql, refundCallback, &data)) {
        std::cerr << "Failed to load refunds" << std::endl;
    }
    
    return refunds;
}

bool SQLiteDataManager::saveFraudAlert(const FraudAlert& fraudAlert) {
    std::stringstream ss;
    ss << "INSERT OR REPLACE INTO fraud_alerts (id, transaction_id, risk_level, description, timestamp, reviewed) VALUES ("
       << "'" << fraudAlert.getAlertId() << "', "
       << "'" << fraudAlert.getTransaction().getTransactionId() << "', "
       << static_cast<int>(fraudAlert.getRiskLevel()) << ", "
       << "'" << fraudAlert.getDescription() << "', "
       << "'" << fraudAlert.getTimestamp() << "', "
       << (fraudAlert.isReviewed() ? 1 : 0)
       << ");";
    
    return executeSQL(ss.str());
}

// Structure to hold fraud alert data during loading
struct FraudAlertData {
    std::vector<std::unique_ptr<FraudAlert>>* fraudAlerts;
    const std::vector<std::unique_ptr<Transaction>>* transactions;
    SQLiteDataManager* manager;
};

// Callback function for loadFraudAlerts
static int fraudAlertCallback(void* data, int argc, char** argv, char** azColName) {
    FraudAlertData* fraudAlertData = static_cast<FraudAlertData*>(data);
    
    std::string id = argv[0] ? argv[0] : "";
    std::string transactionId = argv[1] ? argv[1] : "";
    int riskLevel = argv[2] ? std::stoi(argv[2]) : 0;
    std::string description = argv[3] ? argv[3] : "";
    std::string timestamp = argv[4] ? argv[4] : "";
    bool reviewed = argv[5] ? (std::stoi(argv[5]) != 0) : false;
    
    // Find the transaction
    const Transaction* transaction = fraudAlertData->manager->findTransactionById(transactionId, *fraudAlertData->transactions);
    
    if (!transaction) {
        std::cerr << "Failed to find transaction for fraud alert " << id << std::endl;
        return 0;
    }
    
    // Create the fraud alert
    auto fraudAlert = FraudAlertFactory::createFraudAlert(
        *transaction, static_cast<FraudRiskLevel>(riskLevel), description);
    
    // Set the reviewed status
    fraudAlert->setReviewed(reviewed);
    
    fraudAlertData->fraudAlerts->push_back(std::move(fraudAlert));
    
    return 0;
}

std::vector<std::unique_ptr<FraudAlert>> SQLiteDataManager::loadFraudAlerts(
    const std::vector<std::unique_ptr<Transaction>>& transactions) {
    
    std::vector<std::unique_ptr<FraudAlert>> fraudAlerts;
    
    FraudAlertData data;
    data.fraudAlerts = &fraudAlerts;
    data.transactions = &transactions;
    data.manager = this;
    
    std::string sql = "SELECT id, transaction_id, risk_level, description, timestamp, reviewed FROM fraud_alerts;";
    
    if (!executeQuery(sql, fraudAlertCallback, &data)) {
        std::cerr << "Failed to load fraud alerts" << std::endl;
    }
    
    return fraudAlerts;
}

const Customer* SQLiteDataManager::findCustomerByName(const std::string& name, const std::vector<Customer>& customers) const {
    for (const auto& customer : customers) {
        if (customer.getName() == name) {
            return &customer;
        }
    }
    
    return nullptr;
}

const Merchant* SQLiteDataManager::findMerchantByName(const std::string& name, const std::vector<Merchant>& merchants) const {
    for (const auto& merchant : merchants) {
        if (merchant.getName() == name) {
            return &merchant;
        }
    }
    
    return nullptr;
}

const Transaction* SQLiteDataManager::findTransactionById(const std::string& id, const std::vector<std::unique_ptr<Transaction>>& transactions) const {
    for (const auto& transaction : transactions) {
        if (transaction->getTransactionId() == id) {
            return transaction.get();
        }
    }
    
    return nullptr;
}

std::unique_ptr<PaymentMethod> SQLiteDataManager::createPaymentMethod(
    const std::string& type,
    const std::string& details1,
    const std::string& details2,
    const std::string& details3,
    const std::string& details4) const {
    
    if (type == "Credit Card") {
        return PaymentMethodFactory::createCreditCard(details1, details2, details3, details4);
    } else if (type == "Debit Card") {
        return PaymentMethodFactory::createDebitCard(details1, details2, details3, details4);
    } else if (type == "Digital Wallet") {
        return PaymentMethodFactory::createDigitalWallet(details1, details2);
    }
    
    return nullptr;
}
