# Design Principles in SecurePay Payment Processing System

This document provides a detailed analysis of the design principles applied in the SecurePay Payment Processing System, with specific code point references demonstrating how each principle is implemented.

## 1. Single Responsibility Principle (SRP)

**Definition**: A class should have only one reason to change, meaning it should have only one responsibility.

**Implementation in SecurePay**:

- **Transaction Class**: Responsible solely for representing transaction data and state.
  ```cpp
  // src/core/transaction.h
  class Transaction {
  private:
      std::string m_transactionId;
      const Customer& m_customer;
      const Merchant& m_merchant;
      std::unique_ptr<PaymentMethod> m_paymentMethod;
      double m_amount;
      std::unique_ptr<TransactionState> m_state;
      // ...
  };
  ```

- **PaymentGateway Class**: Responsible only for processing payments.
  ```cpp
  // src/core/paymentgateway.h
  class PaymentGateway {
  public:
      void processTransaction(std::unique_ptr<Transaction> transaction);
      bool authorizeTransaction(std::unique_ptr<Transaction> transaction);
      bool captureTransaction(const std::string& transactionId, double amount);
      // ...
  };
  ```

- **FraudSystem Class**: Responsible only for fraud detection.
  ```cpp
  // src/core/fraudsystem.h
  class FraudSystem {
  public:
      FraudRiskLevel evaluateTransaction(const Transaction& transaction);
      void reportFraud(const Transaction& transaction);
      // ...
  };
  ```

**Benefits**: Each class has a clear, focused purpose, making the code easier to understand, maintain, and test. Changes to one aspect of the system (e.g., fraud detection) don't require changes to unrelated components (e.g., payment processing).

## 2. Open-Closed Principle (OCP)

**Definition**: Software entities should be open for extension but closed for modification.

**Implementation in SecurePay**:

- **PaymentMethod Hierarchy**: The abstract base class allows adding new payment methods without modifying existing code.
  ```cpp
  // src/core/paymentmethod.h
  class PaymentMethod {
  public:
      virtual ~PaymentMethod() = default;
      virtual bool process(double amount) const = 0;
      virtual std::string getType() const = 0;
      virtual std::string getDetails() const = 0;
      virtual std::string getCVV() const = 0;
      virtual PaymentMethod* clone() const = 0;
  };
  
  // New payment methods can be added by extending the base class
  class CreditCard : public PaymentMethod {
      // Implementation
  };
  
  class DebitCard : public PaymentMethod {
      // Implementation
  };
  
  class DigitalWallet : public PaymentMethod {
      // Implementation
  };
  ```

- **TransactionState Hierarchy**: New transaction states can be added without modifying the Transaction class.
  ```cpp
  // src/core/transaction.cpp
  class TransactionState {
  public:
      virtual bool process(Transaction& transaction) = 0;
      virtual bool authorize(Transaction& transaction) = 0;
      virtual bool refund(Transaction& transaction, double amount) = 0;
      virtual TransactionStatus getStatus() const = 0;
      // ...
  };
  
  // New states can be added by implementing the interface
  class PendingState : public TransactionState {
      // Implementation
  };
  
  class ApprovedState : public TransactionState {
      // Implementation
  };
  ```

**Benefits**: The system can be extended with new functionality (e.g., new payment methods, transaction states) without modifying existing code, reducing the risk of introducing bugs in working code.

## 3. Liskov Substitution Principle (LSP)

**Definition**: Objects of a superclass should be replaceable with objects of a subclass without affecting the correctness of the program.

**Implementation in SecurePay**:

- **Transaction States**: All transaction state classes can be used interchangeably through the TransactionState interface.
  ```cpp
  // src/core/transaction.cpp
  bool Transaction::process() {
      return m_state->process(*this);
  }
  
  bool Transaction::authorize() {
      return m_state->authorize(*this);
  }
  
  bool Transaction::refund(double amount) {
      return m_state->refund(*this, amount);
  }
  ```

- **Payment Methods**: All payment method classes can be used interchangeably through the PaymentMethod interface.
  ```cpp
  // src/core/paymentgateway.cpp
  void PaymentGateway::processTransaction(std::unique_ptr<Transaction> transaction) {
      // The specific payment method type doesn't matter here
      // All payment methods are used through the common interface
      const PaymentMethod& paymentMethod = transaction->getPaymentMethod();
      // ...
  }
  ```

**Benefits**: The system can work with different implementations of interfaces without knowing the specific types, enabling polymorphic behavior and making the code more flexible and extensible.

## 4. Interface Segregation Principle (ISP)

**Definition**: No client should be forced to depend on methods it does not use.

**Implementation in SecurePay**:

- **DataManager Interface**: Defines only the methods needed by clients.
  ```cpp
  // src/core/datamanager.h
  class DataManager {
  public:
      virtual ~DataManager() = default;
      virtual bool initialize() = 0;
      virtual bool saveCustomer(const Customer& customer) = 0;
      virtual bool saveMerchant(const Merchant& merchant) = 0;
      virtual bool saveTransaction(const Transaction& transaction) = 0;
      virtual bool saveCardToken(const CardToken& cardToken) = 0;
      virtual bool deleteCardToken(const std::string& token) = 0;
      virtual std::vector<Customer> loadCustomers() = 0;
      virtual std::vector<Merchant> loadMerchants() = 0;
      // ...
  };
  ```

- **TransactionObserver Interface**: Defines only the methods needed for transaction observation.
  ```cpp
  // src/core/paymentgateway.h
  class TransactionObserver {
  public:
      virtual void onTransactionUpdated(const Transaction& transaction) = 0;
  };
  ```

**Benefits**: Clients only need to implement the methods they actually use, reducing the impact of changes to interfaces and preventing forced implementation of unnecessary methods.

## 5. Dependency Inversion Principle (DIP)

**Definition**: High-level modules should not depend on low-level modules. Both should depend on abstractions.

**Implementation in SecurePay**:

- **AppController and DataManager**: AppController depends on the DataManager interface, not on the concrete SQLiteDataManager.
  ```cpp
  // src/core/appcontroller.h
  class AppController {
  private:
      std::unique_ptr<DataManager> m_dataManager;
      
  public:
      AppController() {
          // Initialize with a specific implementation
          m_dataManager = std::make_unique<SQLiteDataManager>("securepay.db");
      }
      // ...
  };
  ```

- **ReportManager and Report**: ReportManager depends on the Report interface, not on concrete report implementations.
  ```cpp
  // src/core/reportmanager.h
  class ReportManager {
  private:
      std::map<std::string, std::unique_ptr<Report>> m_reports;
      
  public:
      std::string generateReport(const std::string& reportType) {
          // Works with the Report interface, not concrete implementations
          if (m_reports.find(reportType) == m_reports.end()) {
              m_reports[reportType] = std::make_unique<LazyReport>(reportType);
          }
          
          return m_reports[reportType]->generate();
      }
      // ...
  };
  ```

**Benefits**: High-level modules are decoupled from low-level implementation details, making the system more flexible and easier to test. Changes to implementations don't affect the high-level modules.

## 6. Law of Demeter

**Definition**: A module should not know about the inner details of the objects it manipulates (principle of least knowledge).

**Implementation in SecurePay**:

- **PaymentGatewayFacade**: Provides a simplified interface to the payment subsystem, hiding the interactions between PaymentGateway, Bank, and FraudSystem.
  ```cpp
  // src/core/paymentgatewayfacade.h
  class PaymentGatewayFacade {
  private:
      PaymentGateway& m_paymentGateway;
      Bank& m_bank;
      FraudSystem& m_fraudSystem;
      
  public:
      PaymentGatewayFacade(PaymentGateway& paymentGateway, Bank& bank, FraudSystem& fraudSystem)
          : m_paymentGateway(paymentGateway), m_bank(bank), m_fraudSystem(fraudSystem) {}
      
      bool processPayment(const Customer& customer, const Merchant& merchant,
                         std::unique_ptr<PaymentMethod> paymentMethod, double amount) {
          // Handles the interactions between components internally
          // Clients don't need to know about these interactions
          // ...
      }
  };
  ```

- **Transaction Class**: Delegates state-specific behavior to the state object without exposing the state's internal details.
  ```cpp
  // src/core/transaction.cpp
  bool Transaction::process() {
      // Delegates to the state object without knowing its internal details
      return m_state->process(*this);
  }
  ```

**Benefits**: Reduces coupling between components, making the system more maintainable and less prone to cascading changes when one component changes.

## 7. Composition over Inheritance

**Definition**: Favor object composition over class inheritance for code reuse.

**Implementation in SecurePay**:

- **Transaction States**: Transaction uses composition to delegate behavior to state objects rather than inheriting from different state classes.
  ```cpp
  // src/core/transaction.h
  class Transaction {
  private:
      std::unique_ptr<TransactionState> m_state;
      
  public:
      void setState(std::unique_ptr<TransactionState> state) {
          m_state = std::move(state);
      }
      
      bool process() {
          return m_state->process(*this);
      }
      // ...
  };
  ```

- **TransactionDecorator**: Uses composition to wrap a Transaction object and add functionality.
  ```cpp
  // src/core/transactiondecorator.h
  class TransactionDecorator : public Transaction {
  protected:
      std::unique_ptr<Transaction> m_wrappedTransaction;
      
  public:
      TransactionDecorator(std::unique_ptr<Transaction> transaction)
          : m_wrappedTransaction(std::move(transaction)) {}
      
      // Override methods to add functionality while delegating to the wrapped transaction
      // ...
  };
  ```

**Benefits**: Provides more flexibility than inheritance, allowing behavior to be changed at runtime and avoiding the problems of deep inheritance hierarchies.

## 8. Programming by Contract

**Definition**: Functions and methods have documented preconditions, postconditions, and invariants.

**Implementation in SecurePay**:

- **Method Documentation**: Methods have clear documentation specifying their expected inputs and outputs.
  ```cpp
  // src/core/customer.h
  /**
   * @brief Deduct an amount from a payment method balance
   * @param method The payment method type
   * @param amount The amount to deduct
   * @return True if successful, false if insufficient funds
   */
  bool deduct(const std::string& method, double amount);
  ```

- **Input Validation**: Methods validate their inputs to ensure preconditions are met.
  ```cpp
  // src/core/transaction.cpp
  bool ApprovedState::refund(Transaction& transaction, double amount) {
      // Precondition: amount must be positive and not greater than the remaining amount
      if (amount <= 0) {
          std::cout << "Refund amount must be positive" << std::endl;
          return false;
      }
      
      if (amount > transaction.getRemainingAmount()) {
          std::cout << "Cannot refund more than the remaining amount" << std::endl;
          return false;
      }
      
      // Process the refund
      // ...
      
      // Postcondition: the refunded amount is added to the transaction's refunded amount
      transaction.addRefundedAmount(amount);
      
      // ...
  }
  ```

**Benefits**: Makes the code more robust by clearly defining the expected behavior and validating inputs and outputs, reducing the likelihood of bugs and making the code easier to understand and use correctly.

## 9. Lazy Evaluation

**Definition**: Defer expensive operations until their results are actually needed.

**Implementation in SecurePay**:

- **LazyReport Class**: Defers report generation until the report is actually requested.
  ```cpp
  // src/core/lazyreport.h
  class LazyReport : public Report {
  private:
      std::unique_ptr<Report> m_realReport;
      std::string m_reportType;
      std::string m_cachedReport;
      bool m_isGenerated;
      
  public:
      LazyReport(const std::string& reportType)
          : m_reportType(reportType), m_isGenerated(false) {}
      
      std::string generate() override {
          if (!m_isGenerated) {
              // Create the real report only when needed
              if (m_reportType == "transaction") {
                  m_realReport = std::make_unique<TransactionReport>();
              } else if (m_reportType == "customer") {
                  m_realReport = std::make_unique<CustomerReport>();
              }
              
              if (m_realReport) {
                  m_cachedReport = m_realReport->generate();
                  m_isGenerated = true;
              }
          }
          
          return m_cachedReport;
      }
  };
  ```

- **CardToken Loading**: Card tokens are loaded from the database only when needed.
  ```cpp
  // src/core/appcontroller.cpp
  std::vector<const CardToken*> AppController::getCardTokensForCustomer(const std::string& customerId) const {
      // Card tokens are loaded from the database only when requested
      return CardManager::getInstance().getCardTokensForCustomer(customerId);
  }
  ```

**Benefits**: Improves performance by avoiding unnecessary computations and resource usage, especially for expensive operations that might not be needed.

## 10. Principle of Least Astonishment

**Definition**: Software should behave in a way that users expect, without surprising or confusing them.

**Implementation in SecurePay**:

- **GUI Consistency**: UI components follow consistent patterns and behaviors.
  ```cpp
  // src/gui/customerview.cpp
  void CustomerView::onSubmitClicked() {
      // Validate inputs in a consistent manner
      bool amountValid;
      double amount = m_amountEdit->text().toDouble(&amountValid);
      
      if (!amountValid || amount <= 0) {
          QMessageBox::warning(this, "Validation Error", "Please enter a valid amount.");
          m_amountEdit->setFocus();
          return;
      }
      
      // Similar validation for other fields
      // ...
      
      // Clear fields after submission (consistent behavior)
      m_amountEdit->clear();
      m_cardNumberEdit->clear();
      m_cardholderNameEdit->clear();
      m_expiryDateEdit->clear();
      m_cvvEdit->clear();
      // ...
  }
  ```

- **Error Handling**: Consistent error messages and handling throughout the application.
  ```cpp
  // src/gui/customerview.cpp
  void CustomerView::onDepositClicked() {
      // Consistent validation and error messaging
      if (m_currentCustomerIndex < 0 || m_currentCustomerIndex >= static_cast<int>(m_appController.getCustomers().size())) {
          QMessageBox::warning(this, "Validation Error", "Please select a customer.");
          return;
      }
      
      bool amountValid;
      double amount = m_depositAmountEdit->text().toDouble(&amountValid);
      
      if (!amountValid || amount <= 0) {
          QMessageBox::warning(this, "Validation Error", "Please enter a valid amount.");
          m_depositAmountEdit->setFocus();
          return;
      }
      
      // ...
  }
  ```

**Benefits**: Makes the application more intuitive and user-friendly, reducing the learning curve and user errors.

## 11. Factory Principle

**Definition**: Use factory methods or classes to create objects instead of direct instantiation.

**Implementation in SecurePay**:

- **PaymentMethodFactory**: Creates payment method objects based on the type and details.
  ```cpp
  // src/core/paymentmethod.h
  class PaymentMethodFactory {
  public:
      static std::unique_ptr<PaymentMethod> createCreditCard(
          const std::string& cardNumber, const std::string& cardholderName,
          const std::string& expiryDate, const std::string& cvv);
          
      static std::unique_ptr<PaymentMethod> createDebitCard(
          const std::string& cardNumber, const std::string& cardholderName,
          const std::string& expiryDate, const std::string& cvv);
          
      static std::unique_ptr<PaymentMethod> createDigitalWallet(
          const std::string& walletId, const std::string& email);
  };
  
  // src/core/paymentmethod.cpp
  std::unique_ptr<PaymentMethod> PaymentMethodFactory::createCreditCard(
      const std::string& cardNumber, const std::string& cardholderName,
      const std::string& expiryDate, const std::string& cvv) {
      return std::make_unique<CreditCard>(cardNumber, cardholderName, expiryDate, cvv);
  }
  ```

- **TransactionFactory**: Creates transaction objects with the appropriate configuration.
  ```cpp
  // src/core/transaction.h
  class TransactionFactory {
  public:
      static std::unique_ptr<Transaction> createTransaction(
          const Customer& customer, const Merchant& merchant,
          std::unique_ptr<PaymentMethod> paymentMethod, double amount);
          
      static std::unique_ptr<Transaction> createTransactionWithIdempotencyKey(
          const Customer& customer, const Merchant& merchant,
          std::unique_ptr<PaymentMethod> paymentMethod, double amount,
          const std::string& idempotencyKey);
  };
  ```

**Benefits**: Encapsulates object creation logic, making it easier to change how objects are created without affecting client code. It also provides a consistent interface for creating objects.

## 12. Precondition, Postcondition, Class Invariant Principles

**Definition**: Methods should have clearly defined preconditions (what must be true before calling) and postconditions (what will be true after calling), and classes should maintain invariants (conditions that are always true).

**Implementation in SecurePay**:

- **Transaction Class**: Maintains invariants such as the transaction ID being non-empty and the amount being positive.
  ```cpp
  // src/core/transaction.cpp
  Transaction::Transaction(const Customer& customer, const Merchant& merchant, 
                          std::unique_ptr<PaymentMethod> paymentMethod, double amount)
      : m_customer(customer), 
        m_merchant(merchant),
        m_paymentMethod(std::move(paymentMethod)),
        m_amount(amount),
        m_refundedAmount(0.0),
        m_timestamp(std::chrono::system_clock::now()) {
      // Ensure the transaction ID is generated (invariant)
      m_transactionId = generateTransactionId();
      // Ensure the transaction has a valid initial state (invariant)
      m_state = std::make_unique<PendingState>();
  }
  ```

- **Bank::authorizeTransaction**: Has preconditions (valid payment method, sufficient funds) and postconditions (transaction status updated).
  ```cpp
  // src/core/bank.cpp
  AuthorizationResult Bank::authorizeTransaction(const Transaction& transaction, 
                                                FraudRiskLevel fraudRiskLevel) {
      std::cout << "Authorizing transaction " << transaction.getTransactionId() << std::endl;
      
      // Precondition: Payment method must be valid
      if (!isCardValid(transaction.getPaymentMethod())) {
          std::cout << "Card validation failed" << std::endl;
          return AuthorizationResult::DECLINED;
      }
      
      // Precondition: Customer must have sufficient funds
      if (!hasSufficientFunds(transaction)) {
          std::cout << "Insufficient funds" << std::endl;
          return AuthorizationResult::DECLINED;
      }
      
      // Process based on fraud risk level
      if (fraudRiskLevel == FraudRiskLevel::HIGH) {
          std::cout << "High fraud risk, review required" << std::endl;
          return AuthorizationResult::REVIEW_REQUIRED;
      } else if (fraudRiskLevel == FraudRiskLevel::MEDIUM) {
          std::cout << "Medium fraud risk, but approved" << std::endl;
          return AuthorizationResult::APPROVED;
      }
      
      // Postcondition: Transaction is approved if all checks pass
      std::cout << "Transaction approved" << std::endl;
      return AuthorizationResult::APPROVED;
  }
  ```

**Benefits**: Makes the code more robust and easier to understand by clearly defining the expected behavior and constraints. It also helps with debugging by making it clear when a method is being used incorrectly.

## Conclusion

The SecurePay Payment Processing System demonstrates a strong commitment to software design principles. By adhering to these principles, the system achieves:

1. **Modularity**: Components are well-separated with clear responsibilities.
2. **Extensibility**: New features can be added without modifying existing code.
3. **Maintainability**: Code is organized in a way that makes it easier to understand and modify.
4. **Robustness**: Preconditions, postconditions, and invariants ensure the system behaves correctly.
5. **Performance**: Lazy evaluation and other optimizations improve efficiency.

These design principles work together to create a system that is not only functional but also adaptable to changing requirements and easy to maintain over time.
