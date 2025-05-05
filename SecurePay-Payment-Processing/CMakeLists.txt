cmake_minimum_required(VERSION 3.16)
project(SecurePay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)
find_package(SQLite3 REQUIRED)

set(SOURCES
    src/main.cpp
    # Core classes
    src/core/customer.cpp
    src/core/merchant.cpp
    src/core/transaction.cpp
    src/core/paymentmethod.cpp
    src/core/paymentgateway.cpp
    src/core/fraudsystem.cpp
    src/core/bank.cpp
    src/core/appcontroller.cpp
    src/core/refund.cpp
    src/core/fraudalert.cpp
    src/core/refundmanager.cpp
    src/core/reportmanager.cpp
    src/core/sqlitedatamanager.cpp
    src/core/transactiondecorator.cpp
    src/core/paymentgatewayfacade.cpp
    src/core/lazyreport.cpp
    src/core/cardtoken.cpp
    src/core/enhancedfraudsystem.cpp
    # GUI classes
    src/gui/mainwindow.cpp
    src/gui/addcustomerdialog.cpp
    src/gui/addmerchantdialog.cpp
    src/gui/refunddialog.cpp
    src/gui/exportreportdialog.cpp
    src/gui/addcarddialog.cpp
    src/gui/managecardsdialog.cpp
    src/gui/checkoutscreen.cpp
)

set(HEADERS
    # Core classes
    src/core/customer.h
    src/core/merchant.h
    src/core/transaction.h
    src/core/paymentmethod.h
    src/core/paymentgateway.h
    src/core/fraudsystem.h
    src/core/bank.h
    src/core/appcontroller.h
    src/core/refund.h
    src/core/fraudalert.h
    src/core/refundmanager.h
    src/core/reportmanager.h
    src/core/datamanager.h
    src/core/sqlitedatamanager.h
    src/core/transactiondecorator.h
    src/core/paymentgatewayfacade.h
    src/core/lazyreport.h
    src/core/cardtoken.h
    src/core/enhancedfraudsystem.h
    # GUI classes
    src/gui/mainwindow.h
    src/gui/addcustomerdialog.h
    src/gui/addmerchantdialog.h
    src/gui/refunddialog.h
    src/gui/exportreportdialog.h
    src/gui/addcarddialog.h
    src/gui/managecardsdialog.h
    src/gui/checkoutscreen.h
)

add_executable(SecurePay ${SOURCES} ${HEADERS})

target_include_directories(SecurePay PRIVATE
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(SecurePay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    ${SQLite3_LIBRARIES}
)

# Install SQLite3 DLL on Windows
if(WIN32)
    add_custom_command(TARGET SecurePay POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SQLite3_LIBRARIES} $<TARGET_FILE_DIR:SecurePay>
    )
endif()
