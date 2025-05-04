# SecurePay Payment Processing System

SecurePay is a comprehensive payment processing application with finance tracking capabilities, designed to demonstrate strong object-oriented design principles and patterns.

## Features

### Customer Side
- Select existing customer or add new (name, email, address)
- Make payment (amount, payment method: CreditCard, DebitCard, Wallet)
- View transaction history (table view)
- View spending summaries (pie chart, bar graph)
- Export transaction history (CSV, JSON)

### Merchant Side
- Select/add merchant
- View incoming payments
- Issue full/partial refunds
- View refund history
- View daily/monthly earnings summaries
- View fraud alerts (flagged transactions)
- Export income, refunds, and fraud reports (CSV, JSON)

### Fraud Detection
- Real-time fraud scoring based on rules
- Flag suspicious transactions
- Merchant dashboard highlights fraud alerts

### Persistence
- Customers, Merchants, Transactions, Refunds, Fraud Alerts saved
- SQLite database for data persistence
- Data loaded at startup and saved on exit

## Design Principles & Patterns

### Design Principles
- **Single Responsibility Principle (SRP)**: Each class has a single responsibility (e.g., Transaction handles transaction data, PaymentGateway processes payments)
- **Open-Closed Principle (OCP)**: Classes are open for extension but closed for modification (e.g., PaymentMethod hierarchy)
- **Liskov Substitution Principle (LSP)**: Subclasses are substitutable for base classes (e.g., CreditCard, DebitCard, DigitalWallet)
- **Interface Segregation Principle (ISP)**: Interfaces are client-specific (e.g., DataManager interface)
- **Dependency Inversion Principle (DIP)**: High-level modules depend on abstractions (e.g., ReportManager depends on ReportStrategy)
- **Law of Demeter**: Classes minimize knowledge of other classes (e.g., PaymentGatewayFacade)
- **Composition over Inheritance**: Favoring object composition over class inheritance (e.g., Transaction states)
- **Programming by Contract**: Functions have documented preconditions and postconditions
- **Lazy Evaluation**: Expensive operations deferred until needed (e.g., LazyReport)
- **Principle of Least Astonishment**: GUI actions behave intuitively
- **Factory Principle**: Factories used to create objects
- **Precondition, Postcondition, Class Invariant Principles**: Documented in class and method comments

### Design Patterns
- **Strategy Pattern**: Different payment methods (CreditCard, DebitCard, DigitalWallet) and report generation strategies
- **Factory Method Pattern**: Creating PaymentMethod, Transaction, Refund, and FraudAlert instances
- **Singleton Pattern**: Bank, FraudSystem, ReportManager, and RefundManager
- **Observer Pattern**: GUI updates on transactions through the TransactionObserver interface
- **Command Pattern**: Refund processing with RefundCommand, FullRefundCommand, and PartialRefundCommand
- **Decorator Pattern**: Transaction encryption and logging with TransactionDecorator, EncryptedTransaction, and LoggedTransaction
- **MVC/MVVM Pattern**: Separation of GUI, controllers, and model classes
- **Façade Pattern**: PaymentGatewayFacade simplifies the interface to the payment processing subsystem
- **State Pattern**: Transaction status management with TransactionState and concrete state classes

## Build Instructions

### Prerequisites
- CMake 3.16 or higher
- C++17 compatible compiler
- Qt6 (Core, Gui, Widgets, Charts)
- SQLite3 development libraries

### Building the Project

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/SecurePay-Payment-Processing.git
   cd SecurePay-Payment-Processing
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Configure and build:
   ```
   cmake ..
   make
   ```

4. Run the application:
   ```
   ./SecurePay
   ```

## Usage Instructions

### Customer Operations
1. Select an existing customer or add a new one
2. Enter payment details (amount, payment method)
3. Submit payment
4. View transaction history
5. Export transaction reports

### Merchant Operations
1. Select an existing merchant or add a new one
2. View incoming transactions
3. Process refunds for transactions
4. View earnings summaries
5. Export merchant reports

### Report Generation
1. Select report type (Transaction History, Refund History, etc.)
2. Apply filters (customer, merchant, date range)
3. Choose export format (CSV or JSON)
4. Export the report to a file

## Architecture

The application follows a layered architecture:
- View Layer: Qt6 GUI Forms and Widgets
- Controller Layer: AppController, PaymentController, RefundController
- Business Logic Layer: PaymentGateway, FraudSystem, Bank, RefundManager, ReportManager
- Model Layer: Customer, Merchant, Transaction, Refund, PaymentMethod
- Persistence Layer: DataManager (SQLite implementation)

## License

This project is licensed under the MIT License - see the LICENSE file for details.
