find_package(Qt6 COMPONENTS Test REQUIRED)

# Usage: securepay_add_test(tst_name) builds tests/tst_name.cpp against securepay_core
function(securepay_add_test name)
    add_executable(${name} ${name}.cpp)
    set_target_properties(${name} PROPERTIES AUTOMOC ON)
    target_link_libraries(${name} PRIVATE securepay_core Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

securepay_add_test(tst_paymentmethod)
securepay_add_test(tst_bank)
securepay_add_test(tst_fraudsystem)
securepay_add_test(tst_paymentgateway)
securepay_add_test(tst_reportmanager)
securepay_add_test(tst_refundmanager)
securepay_add_test(tst_transaction)
