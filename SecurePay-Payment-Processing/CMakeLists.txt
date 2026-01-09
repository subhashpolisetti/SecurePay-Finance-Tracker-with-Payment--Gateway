cmake_minimum_required(VERSION 3.16)
project(SecurePay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)
find_package(SQLite3 REQUIRED)

# Core payment logic: plain C++17 + SQLite, no Qt dependency
set(CORE_SOURCES
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
)

set(CORE_HEADERS
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
)

add_library(securepay_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(securepay_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(securepay_core PUBLIC SQLite::SQLite3)

# Qt desktop application
set(GUI_SOURCES
    src/main.cpp
    src/gui/mainwindow.cpp
    src/gui/addcustomerdialog.cpp
    src/gui/addmerchantdialog.cpp
    src/gui/refunddialog.cpp
    src/gui/exportreportdialog.cpp
)

set(GUI_HEADERS
    src/gui/mainwindow.h
    src/gui/addcustomerdialog.h
    src/gui/addmerchantdialog.h
    src/gui/refunddialog.h
    src/gui/exportreportdialog.h
)

add_executable(SecurePay ${GUI_SOURCES} ${GUI_HEADERS})

set_target_properties(SecurePay PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_link_libraries(SecurePay PRIVATE
    securepay_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
)

# Install SQLite3 DLL on Windows
if(WIN32)
    add_custom_command(TARGET SecurePay POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SQLite3_LIBRARIES} $<TARGET_FILE_DIR:SecurePay>
    )
endif()
