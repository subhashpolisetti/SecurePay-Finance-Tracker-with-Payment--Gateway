#include <QApplication>
#include "gui/mainwindow.h"
#include <iostream>
#include <exception>

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        
        QApplication::setApplicationName("SecurePay");
        QApplication::setApplicationVersion("1.0");
        QApplication::setOrganizationName("CMPE 202");
        
        std::cout << "Starting SecurePay Payment Processing Application" << std::endl;
        
        MainWindow mainWindow;
        mainWindow.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
