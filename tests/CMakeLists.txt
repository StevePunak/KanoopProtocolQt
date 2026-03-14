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
