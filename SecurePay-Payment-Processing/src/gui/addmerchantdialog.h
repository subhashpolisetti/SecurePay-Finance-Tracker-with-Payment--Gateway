#ifndef ADDMERCHANTDIALOG_H
#define ADDMERCHANTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../core/merchant.h"

class AddMerchantDialog : public QDialog {
    Q_OBJECT
    
public:
    AddMerchantDialog(QWidget* parent = nullptr);
    
    Merchant getMerchant() const;
    
private:
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_addressEdit;
    
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
private slots:
    void validateInput();
};

#endif // ADDMERCHANTDIALOG_H
