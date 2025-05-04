#ifndef ADDCUSTOMERDIALOG_H
#define ADDCUSTOMERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include "../core/customer.h"

class AddCustomerDialog : public QDialog {
    Q_OBJECT
    
public:
    AddCustomerDialog(QWidget* parent = nullptr);
    
    
    Customer getCustomer() const;
    
private:
  
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_addressEdit;
    
   
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
private slots:
  
    void validateForm();
};

#endif 