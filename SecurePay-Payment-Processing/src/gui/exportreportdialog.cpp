#include "exportreportdialog.h"
#include <QMessageBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>

ExportReportDialog::ExportReportDialog(ReportManager& reportManager, QWidget* parent)
    : QDialog(parent), m_reportManager(reportManager) {
    
    setWindowTitle("Export Report");
    setMinimumWidth(500);
    
    setupUI();
}

void ExportReportDialog::setFilterCriteria(const std::map<std::string, std::string>& criteria) {
    // Set the filter criteria in the UI
    auto it = criteria.find("customerId");
    if (it != criteria.end()) {
        m_customerIdEdit->setText(QString::fromStdString(it->second));
        m_customerIdEdit->setEnabled(false); // Disable editing
    }
    
    it = criteria.find("merchantId");
    if (it != criteria.end()) {
        m_merchantIdEdit->setText(QString::fromStdString(it->second));
        m_merchantIdEdit->setEnabled(false); // Disable editing
    }
    
    it = criteria.find("startDate");
    if (it != criteria.end()) {
        QDate date = QDate::fromString(QString::fromStdString(it->second), "yyyy-MM-dd");
        if (date.isValid()) {
            m_startDateEdit->setDate(date);
            m_startDateEdit->setEnabled(false); // Disable editing
        }
    }
    
    it = criteria.find("endDate");
    if (it != criteria.end()) {
        QDate date = QDate::fromString(QString::fromStdString(it->second), "yyyy-MM-dd");
        if (date.isValid()) {
            m_endDateEdit->setDate(date);
            m_endDateEdit->setEnabled(false); // Disable editing
        }
    }
}

void ExportReportDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Report type selection
    QGroupBox* reportTypeGroup = new QGroupBox("Report Type", this);
    QVBoxLayout* reportTypeLayout = new QVBoxLayout(reportTypeGroup);
    
    m_reportTypeComboBox = new QComboBox(reportTypeGroup);
    m_reportTypeComboBox->addItem("Transaction History", static_cast<int>(ReportType::TRANSACTION_HISTORY));
    m_reportTypeComboBox->addItem("Refund History", static_cast<int>(ReportType::REFUND_HISTORY));
    m_reportTypeComboBox->addItem("Fraud Alerts", static_cast<int>(ReportType::FRAUD_ALERTS));
    m_reportTypeComboBox->addItem("Customer Spending", static_cast<int>(ReportType::CUSTOMER_SPENDING));
    m_reportTypeComboBox->addItem("Merchant Earnings", static_cast<int>(ReportType::MERCHANT_EARNINGS));
    m_reportTypeComboBox->addItem("Daily Summary", static_cast<int>(ReportType::DAILY_SUMMARY));
    m_reportTypeComboBox->addItem("Monthly Summary", static_cast<int>(ReportType::MONTHLY_SUMMARY));
    
    reportTypeLayout->addWidget(m_reportTypeComboBox);
    
    // Export format selection
    QGroupBox* formatGroup = new QGroupBox("Export Format", this);
    QHBoxLayout* formatLayout = new QHBoxLayout(formatGroup);
    
    QButtonGroup* formatButtonGroup = new QButtonGroup(formatGroup);
    m_csvRadio = new QRadioButton("CSV", formatGroup);
    m_jsonRadio = new QRadioButton("JSON", formatGroup);
    formatButtonGroup->addButton(m_csvRadio);
    formatButtonGroup->addButton(m_jsonRadio);
    m_csvRadio->setChecked(true);
    
    formatLayout->addWidget(m_csvRadio);
    formatLayout->addWidget(m_jsonRadio);
    formatLayout->addStretch();
    
    // Filter options
    QGroupBox* filterGroup = new QGroupBox("Filter Options", this);
    filterGroup->setObjectName("filterGroup");
    QFormLayout* filterLayout = new QFormLayout(filterGroup);
    
    m_customerIdEdit = new QLineEdit(filterGroup);
    m_merchantIdEdit = new QLineEdit(filterGroup);
    m_startDateEdit = new QDateEdit(filterGroup);
    m_endDateEdit = new QDateEdit(filterGroup);
    
    m_startDateEdit->setCalendarPopup(true);
    m_endDateEdit->setCalendarPopup(true);
    
    // Set default dates to current month
    QDate currentDate = QDate::currentDate();
    QDate firstDayOfMonth(currentDate.year(), currentDate.month(), 1);
    QDate lastDayOfMonth = firstDayOfMonth.addMonths(1).addDays(-1);
    
    m_startDateEdit->setDate(firstDayOfMonth);
    m_endDateEdit->setDate(lastDayOfMonth);
    
    filterLayout->addRow("Customer ID:", m_customerIdEdit);
    filterLayout->addRow("Merchant ID:", m_merchantIdEdit);
    filterLayout->addRow("Start Date:", m_startDateEdit);
    filterLayout->addRow("End Date:", m_endDateEdit);
    
    // File path selection
    QGroupBox* fileGroup = new QGroupBox("Output File", this);
    QHBoxLayout* fileLayout = new QHBoxLayout(fileGroup);
    
    m_filePathEdit = new QLineEdit(fileGroup);
    m_browseButton = new QPushButton("Browse...", fileGroup);
    
    fileLayout->addWidget(m_filePathEdit);
    fileLayout->addWidget(m_browseButton);
    
    // Set default file path
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_filePathEdit->setText(QDir::toNativeSeparators(documentsPath + "/report.csv"));
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_exportButton = new QPushButton("Export", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_cancelButton);
    
    // Main layout
    mainLayout->addWidget(reportTypeGroup);
    mainLayout->addWidget(formatGroup);
    mainLayout->addWidget(filterGroup);
    mainLayout->addWidget(fileGroup);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_reportTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ExportReportDialog::onReportTypeSelected);
    connect(m_browseButton, &QPushButton::clicked,
            this, &ExportReportDialog::onBrowseClicked);
    connect(m_exportButton, &QPushButton::clicked,
            this, &ExportReportDialog::exportReport);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    connect(m_csvRadio, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            QString path = m_filePathEdit->text();
            if (path.endsWith(".json", Qt::CaseInsensitive)) {
                path.replace(path.length() - 5, 5, ".csv");
                m_filePathEdit->setText(path);
            }
        }
    });
    connect(m_jsonRadio, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            QString path = m_filePathEdit->text();
            if (path.endsWith(".csv", Qt::CaseInsensitive)) {
                path.replace(path.length() - 4, 4, ".json");
                m_filePathEdit->setText(path);
            }
        }
    });
    
    // Initial setup
    onReportTypeSelected(m_reportTypeComboBox->currentIndex());
}

void ExportReportDialog::onReportTypeSelected(int index) {
    ReportType reportType = static_cast<ReportType>(m_reportTypeComboBox->currentData().toInt());
    
    // Enable/disable filter options based on report type
    bool showCustomerId = (reportType == ReportType::TRANSACTION_HISTORY ||
                          reportType == ReportType::REFUND_HISTORY ||
                          reportType == ReportType::CUSTOMER_SPENDING);
    
    bool showMerchantId = (reportType == ReportType::TRANSACTION_HISTORY ||
                          reportType == ReportType::REFUND_HISTORY ||
                          reportType == ReportType::MERCHANT_EARNINGS ||
                          reportType == ReportType::DAILY_SUMMARY ||
                          reportType == ReportType::MONTHLY_SUMMARY);
    
    bool showDates = true; // All report types can use date filtering
    
    m_customerIdEdit->setEnabled(showCustomerId);
    m_merchantIdEdit->setEnabled(showMerchantId);
    m_startDateEdit->setEnabled(showDates);
    m_endDateEdit->setEnabled(showDates);
}

void ExportReportDialog::onBrowseClicked() {
    QString filter;
    if (m_csvRadio->isChecked()) {
        filter = "CSV Files (*.csv);;All Files (*)";
    } else {
        filter = "JSON Files (*.json);;All Files (*)";
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Report",
        m_filePathEdit->text(),
        filter
    );
    
    if (!filePath.isEmpty()) {
        m_filePathEdit->setText(filePath);
    }
}

void ExportReportDialog::exportReport() {
    // Get report type
    ReportType reportType = static_cast<ReportType>(m_reportTypeComboBox->currentData().toInt());
    
    // Get export format
    ExportFormat format = m_csvRadio->isChecked() ? ExportFormat::CSV : ExportFormat::JSON;
    
    // Get file path
    QString filePath = m_filePathEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please specify an output file path");
        return;
    }
    
    // Add extension if missing
    if (format == ExportFormat::CSV && !filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
        m_filePathEdit->setText(filePath);
    } else if (format == ExportFormat::JSON && !filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
        m_filePathEdit->setText(filePath);
    }
    
    // Build filter criteria
    std::map<std::string, std::string> filterCriteria;
    
    if (!m_customerIdEdit->text().isEmpty() && m_customerIdEdit->isEnabled()) {
        filterCriteria["customerId"] = m_customerIdEdit->text().toUtf8().constData();
    }
    
    if (!m_merchantIdEdit->text().isEmpty() && m_merchantIdEdit->isEnabled()) {
        filterCriteria["merchantId"] = m_merchantIdEdit->text().toUtf8().constData();
    }
    
    if (m_startDateEdit->isEnabled()) {
        filterCriteria["startDate"] = m_startDateEdit->date().toString("yyyy-MM-dd").toUtf8().constData();
    }
    
    if (m_endDateEdit->isEnabled()) {
        filterCriteria["endDate"] = m_endDateEdit->date().toString("yyyy-MM-dd").toUtf8().constData();
    }
    
    // Generate and export the report
    bool success = m_reportManager.generateAndExportReport(
        reportType,
        filePath.toUtf8().constData(),
        format,
        filterCriteria
    );
    
    if (success) {
        QMessageBox::information(this, "Success", "Report exported successfully");
        accept();
    } else {
        QMessageBox::critical(this, "Error", "Failed to export report");
    }
}
