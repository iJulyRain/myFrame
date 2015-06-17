aux_source_directory(. udp_client)

set(TARGET udp_client_test)

add_executable(${TARGET} ${udp_client})

target_link_libraries(${TARGET} myevent)
