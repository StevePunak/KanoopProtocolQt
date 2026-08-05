find_package(Qt6 REQUIRED COMPONENTS Test Network Mqtt)

function(add_protocol_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        Qt6::Network
        Qt6::Mqtt
        KanoopProtocolQt
        KanoopCommonQt
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_protocol_test(tst_mqtt_parameters)
add_protocol_test(tst_http)
add_protocol_test(tst_http_integration)

# tst_http_integration is opt-in (KANOOP_RUN_NETWORK_TESTS, see the comment at the
# top of the source file) and exits 0 whether it ran or skipped. Without this, a
# default `ctest` run would silently report it as "Passed" even though nothing was
# actually exercised. Recognize QtTest's SKIP output so ctest surfaces it as
# "Not Run" instead.
set_tests_properties(tst_http_integration PROPERTIES
    SKIP_REGULAR_EXPRESSION "SKIP   :"
)
