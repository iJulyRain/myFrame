aux_source_directory(. timestamp)

set(TARGET timestamp_test)

add_executable(${TARGET} ${timestamp})

target_link_libraries(${TARGET} myevent)
