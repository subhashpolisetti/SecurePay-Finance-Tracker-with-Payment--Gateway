#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include "customer.h"
#include "merchant.h"
#include "transaction.h"
#include "paymentmethod.h"
#include "paymentgateway.h"

// Controller class for the application
class AppController : public TransactionObserver {
public:
    AppController();
    ~AppController();
    

    void addCustomer(const Customer& customer);
    const std::vector<Customer>& getCustomers() const;
    std::vector<Customer>& getCustomersMutable();
    
    void addMerchant(const Merchant& merchant);
    const std::vector<Merchant>& getMerchants() const;
    
   
    std::unique_ptr<Transaction> createTransaction(
        const Customer& customer,
        const Merchant& merchant,
        const std::string& paymentMethodType,
        const std::string& paymentDetails1,
        const std::string& paymentDetails2,
        const std::string& paymentDetails3,
        const std::string& paymentDetails4,
        double amount);
    
    void processTransaction(std::unique_ptr<Transaction> transaction);
    
    
    const std::vector<std::unique_ptr<Transaction>>& getTransactionHistory() const;
    
    
    void onTransactionUpdated(const Transaction& transaction) override;
    
    
    void setTransactionUpdateCallback(std::function<void(const Transaction&)> callback);
    
private:
 
    std::vector<Customer> m_customers;
    std::vector<Merchant> m_merchants;
    
 
    std::unique_ptr<PaymentGateway> m_paymentGateway;
    
    
    std::function<void(const Transaction&)> m_transactionUpdateCallback;
    
    
    std::unique_ptr<PaymentMethod> createPaymentMethod(
        const std::string& paymentMethodType,
        const std::string& details1,
        const std::string& details2,
        const std::string& details3,
        const std::string& details4);
};

#endif
