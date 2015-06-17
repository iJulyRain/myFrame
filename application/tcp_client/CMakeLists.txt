aux_source_directory(. tcp_client)

set(TARGET tcp_client_test)

add_executable(${TARGET} ${tcp_client})

target_link_libraries(${TARGET} myevent)
