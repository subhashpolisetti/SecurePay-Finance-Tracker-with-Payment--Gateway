#ifndef EXPORTREPORTDIALOG_H
#define EXPORTREPORTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QRadioButton>
#include <QDateEdit>
#include <QFileDialog>
#include "../core/reportmanager.h"

/**
 * @class ExportReportDialog
 * @brief Dialog for exporting reports
 * 
 * This class follows the Single Responsibility Principle by focusing only on
 * the UI for exporting reports.
 */
class ExportReportDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param reportManager Reference to the report manager
     * @param parent Parent widget
     */
    ExportReportDialog(ReportManager& reportManager, QWidget* parent = nullptr);
    
private:
    /**
     * @brief Report type combo box
     */
    QComboBox* m_reportTypeComboBox;
    
    /**
     * @brief Export format radio buttons
     */
    QRadioButton* m_csvRadio;
    QRadioButton* m_jsonRadio;
    
    /**
     * @brief Filter options
     */
    QLineEdit* m_customerIdEdit;
    QLineEdit* m_merchantIdEdit;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    
    /**
     * @brief File path edit
     */
    QLineEdit* m_filePathEdit;
    
    /**
     * @brief Browse button
     */
    QPushButton* m_browseButton;
    
    /**
     * @brief Export button
     */
    QPushButton* m_exportButton;
    
    /**
     * @brief Cancel button
     */
    QPushButton* m_cancelButton;
    
    /**
     * @brief Reference to the report manager
     */
    ReportManager& m_reportManager;
    
    /**
     * @brief Setup the UI
     */
    void setupUI();
    
private slots:
    /**
     * @brief Handle report type selection
     * @param index The selected index
     */
    void onReportTypeSelected(int index);
    
    /**
     * @brief Handle browse button click
     */
    void onBrowseClicked();
    
    /**
     * @brief Export the report
     */
    void exportReport();
};

#endif // EXPORTREPORTDIALOG_H
