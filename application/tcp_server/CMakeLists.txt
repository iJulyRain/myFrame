aux_source_directory(. tcp_server)

set(TARGET tcp_server_test)

add_executable(${TARGET} ${tcp_server})

target_link_libraries(${TARGET} myevent)
